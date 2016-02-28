// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager.h"

#include <string>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/model/cli-parser.h"
#include "core/model/event.h"


using sf::core::bin::Manager;
using sf::core::bin::ManagerToDaemon;

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::model::CLIParser;
using sf::core::model::EventSourceRef;


void Manager::connectDaemon() {
  CLIParser*  parser = Static::parser();
  std::string daemon_path = parser->getString("manager-socket");

  Context::sourceManager()->addSource(EventSourceRef(
      new ManagerToDaemon(daemon_path)
  ));
}

void Manager::initialise() {
  this->registerDefaultSourceManager();
  this->connectDaemon();
}
