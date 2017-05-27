// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/spawner.h"

#include "core/cluster/cluster.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/exceptions/base.h"
#include "core/lifecycle/process.h"

#include "core/model/cli-parser.h"
#include "core/model/logger.h"

#include "core/posix/restricted.h"
#include "core/registry/cli-parser.h"
#include "core/state/global.h"


using sf::core::bin::Spawner;

using sf::core::cluster::Cluster;
using sf::core::context::Context;
using sf::core::context::ProxyLogger;
using sf::core::context::Static;

using sf::core::exception::CleanExit;
using sf::core::exception::SfException;
using sf::core::lifecycle::Process;

using sf::core::model::CLIParser;
using sf::core::model::Logger;
using sf::core::model::LogInfo;

using sf::core::posix::Restricted;
using sf::core::registry::CLIParsers;
using sf::core::state::GlobalState;


static ProxyLogger logger("core.bin.spawner.main");


int main(int argc, char** argv) {
  // Initialise static context and trigger process::init lifecycle event.
  Static::initialise(new Restricted());
  Static::options()->setString("log-group", "Spawner");
  Process::Init();

  try {
    // Parse CLI.
    std::string parser_name = CLIParsers::nameParser(argc, argv);
    CLIParser* parser = CLIParsers::Get(parser_name)();

    // Prepare to parse.
    Static::parser(parser);
    CLIParser::miscOptions(parser);
    CLIParser::spawnerOptions(parser);
    parser->parse(&argc, &argv);
    parser->handleVersionOption("snow-fox-spawner");

    // Run spawner.
    Spawner spawner;
    spawner.configureInitialCluster();
    Static::options()->setString(
        "process-name", Cluster::Instance()->myself()->name()
    );
    spawner.initialise();
    spawner.run();

    // NOLINT(whitespace/blank_line)
  } catch (CleanExit& ex) {
    // Ignore and continue.
  } catch (SfException& ex) {
    LogInfo vars = {
      {"error", ex.what()},
      {"trace", ex.getTrace()}
    };
    ERRORV(
        logger,
        "Spawner terminating due to an error: ${error}", vars
    );

    Process::Exit();
    GlobalState::Destroy();
    Logger::destroyFallback();
    Static::destroy();
    return ex.getCode();
  }

  Process::Exit();
  GlobalState::Destroy();
  Logger::destroyFallback();
  Static::destroy();
  return 0;
}
