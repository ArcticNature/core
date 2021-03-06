// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager.h"

#include <string>
#include <utility>

#include "core/bin/manager/event/config.h"
#include "core/cluster/cluster.h"

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/event/drain/null.h"
#include "core/event/source/manual.h"

#include "core/model/cli-parser.h"
#include "core/registry/repositories.h"

#include "core/utility/status.h"


using sf::core::bin::LoadConfiguration;
using sf::core::bin::Manager;
using sf::core::bin::ManagerToDaemon;
using sf::core::bin::ManagerToSpawner;

using sf::core::cluster::Cluster;

using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::NullDrain;
using sf::core::event::ManualSource;

using sf::core::model::CLIParser;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::registry::ReposRegistry;

using sf::core::utility::Status;
using sf::core::utility::StatusLight;


void Manager::connectDaemon() {
  CLIParser*  parser = Static::parser();
  std::string daemon_path = parser->getString("manager-socket");
  std::pair<std::string, std::string> daemon =
    ManagerToDaemon::Connect(daemon_path);
  Static::options()->setString("daemon-source-id", daemon.first);
}

void Manager::connectSpawner() {
  CLIParser*  parser = Static::parser();
  std::string spawner_path = parser->getString("spawner-manager-socket");
  std::pair<std::string, std::string> spawner =
    ManagerToSpawner::Connect(spawner_path);
  Static::options()->setString("spawner-source-id", spawner.first);
  Static::options()->setString("spawner-drain-id", spawner.second);
}

void Manager::defaultSources() {
  Context::LoopManager()->add(EventSourceRef(new ManualSource()));
}

void Manager::initialise() {
  this->disableSIGINT();
  this->registerDefaultSourceManager();
  this->defaultSources();
  this->connectDaemon();
  this->connectSpawner();
  ReposRegistry::initStaticContext();

  // Enqueue initial configuration.
  CLIParser*  parser   = Static::parser();
  std::string conf_ver = parser->getString("repo-version");
  EventRef load_config(new LoadConfiguration(
        conf_ver, EventDrainRef(new NullDrain()), true
  ));
  Context::LoopManager()->downcast<ManualSource>("manual")->enqueueEvent(
      load_config
  );

  // Manager process subsystem ready.
  Cluster::Instance()->myself()->status()->set("node", Status(
      StatusLight::GREEN,
      "Manager process successfully initialised"
  ));
}
