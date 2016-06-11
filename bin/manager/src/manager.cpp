// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager.h"

#include <string>

#include "core/bin/manager/api/server.h"
#include "core/cluster/node.h"

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/model/cli-parser.h"
#include "core/model/event.h"


using sf::core::bin::Manager;
using sf::core::bin::ManagerToDaemon;
using sf::core::bin::ManagerToSpawner;
using sf::core::bin::UnixServer;

using sf::core::cluster::Node;
using sf::core::cluster::NodeStatusCode;
using sf::core::cluster::NodeStatusDetail;

using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::model::CLIParser;
using sf::core::model::EventSourceRef;


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

void Manager::initialise() {
  this->disableSIGINT();
  this->registerDefaultSourceManager();
  this->connectDaemon();
  this->connectSpawner();

  // TODO(stefano): remove this when config works.
  EventSourceRef server(new UnixServer("/tmp/snow-fox.socket"));
  Context::sourceManager()->addSource(server);

  // Manager subsystem ready.
  Node::me()->status()->set("process", NodeStatusDetail(
      NodeStatusCode::PROCESS_READY,
      "Manager process successfully initialised"
  ));
}
