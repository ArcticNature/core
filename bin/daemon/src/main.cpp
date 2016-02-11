// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/daemon.h"

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/interface/lifecycle.h"

#include "core/model/cli-parser.h"
#include "core/model/logger.h"

#include "core/posix/restricted.h"
#include "core/registry/cli-parser.h"

using sf::core::bin::Daemon;

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::exception::CleanExit;
using sf::core::exception::SfException;
using sf::core::interface::Lifecycle;

using sf::core::model::CLIParser;
using sf::core::model::Logger;
using sf::core::model::LogInfo;

using sf::core::posix::Restricted;
using sf::core::registry::CLIParsers;


int main(int argc, char** argv) {
  // Initialise static context and trigger process::init lifecycle event.
  Static::initialise(new Restricted());
  Static::options()->setString("log-group", "Daemon");
  Lifecycle::trigger("process::init");

  try {
    // Parse CLI.
    std::string parser_name = CLIParsers::nameParser(argc, argv);
    CLIParser* parser = CLIParsers::Get(parser_name)();

    // Prepare to parse.
    Static::parser(parser);
    CLIParser::configOptions(parser);
    CLIParser::daemonOptions(parser);
    parser->parse(&argc, &argv);

    // Run daemon.
    Daemon daemon;
    daemon.initialise();
    daemon.run();

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
        "Daemon terminating due to an error: ${error}", vars
    );

    Lifecycle::trigger("process::exit");
    Context::destroy();
    Logger::destroyFallback();
    Static::destroy();
    return ex.getCode();
  }

  Lifecycle::trigger("process::exit");
  Context::destroy();
  Logger::destroyFallback();
  Static::destroy();
  return 0;
}
