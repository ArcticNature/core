// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/client.h"

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/exceptions/base.h"
#include "core/lifecycle/process.h"

#include "core/model/cli-parser.h"
#include "core/model/logger.h"

#include "core/posix/user.h"
#include "core/registry/cli-parser.h"


using sf::core::bin::Client;

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::exception::CleanExit;
using sf::core::exception::SfException;
using sf::core::lifecycle::Process;

using sf::core::model::CLIParser;
using sf::core::model::Logger;
using sf::core::model::LogInfo;

using sf::core::posix::User;
using sf::core::registry::CLIParsers;


int main(int argc, char** argv) {
  // Initialise static context and trigger process::init lifecycle event.
  Static::initialise(new User());
  Static::options()->setString("log-group", "Client");
  Process::Init();

  try {
    // Parse CLI.
    std::string parser_name = CLIParsers::nameParser(argc, argv);
    CLIParser* parser = CLIParsers::Get(parser_name)();

    // Prepare to parse.
    Static::parser(parser);
    CLIParser::clusterOptions(parser);
    CLIParser::managerOptions(parser);
    CLIParser::miscOptions(parser);
    parser->parse(&argc, &argv);
    parser->handleVersionOption("snow-fox-client");

    // Run client.
    Client client;
    client.initialise();
    client.run();

    // NOLINT(whitespace/blank_line)
  } catch (CleanExit& ex) {
    // Ignore and continue.
  } catch (SfException& ex) {
    LogInfo vars = {
      {"error", ex.what()},
      {"trace", ex.getTrace()}
    };
    ERRORV(
        Logger::fallback(),
        "Client terminating due to an error: ${error}", vars
    );

    Process::Exit();
    Context::destroy();
    Logger::destroyFallback();
    Static::destroy();
    return ex.getCode();
  }

  Process::Exit();
  Context::destroy();
  Logger::destroyFallback();
  Static::destroy();
  return 0;
}