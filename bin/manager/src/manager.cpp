// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager.h"

#include <string>

#include "core/bin/manager/event/config.h"
#include "core/cluster/node.h"

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/event/source/manual.h"

#include "core/model/cli-parser.h"
#include "core/registry/repositories.h"


using sf::core::bin::LoadConfiguration;
using sf::core::bin::Manager;
using sf::core::bin::ManagerToDaemon;
using sf::core::bin::ManagerToSpawner;

using sf::core::cluster::Node;
using sf::core::cluster::NodeStatusCode;
using sf::core::cluster::NodeStatusDetail;

using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::ManualSource;

using sf::core::model::CLIParser;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::registry::ReposRegistry;


void Manager::connectDaemon() {
  CLIParser*  parser = Static::parser();
  std::string daemon_path = parser->getString("manager-socket");
  ManagerToDaemon::Connect(daemon_path);
}

void Manager::connectSpawner() {
  CLIParser*  parser = Static::parser();
  std::string spawner_path = parser->getString("spawner-manager-socket");
  ManagerToSpawner::Connect(spawner_path);
}

void Manager::defaultSources() {
  Context::sourceManager()->add(EventSourceRef(new ManualSource()));
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
  EventRef load_config(new LoadConfiguration(conf_ver, "NULL", true));
  Context::sourceManager()->get<ManualSource>("manual")->enqueueEvent(
      load_config
  );

  // Manager process subsystem ready.
  Node::me()->status()->set("process", NodeStatusDetail(
      NodeStatusCode::PROCESS_READY,
      "Manager process successfully initialised"
  ));
}
