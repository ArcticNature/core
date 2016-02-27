// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/spawner.h"

#include <string>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/model/cli-parser.h"
#include "core/model/event.h"


using sf::core::bin::Spawner;
using sf::core::bin::SpawnerToDaemon;

using sf::core::context::Static;
using sf::core::context::Context;

using sf::core::model::CLIParser;
using sf::core::model::EventSourceRef;


void Spawner::connectDaemon() {
  CLIParser*  parser = Static::parser();
  std::string daemon_path = parser->getString("spawner-socket");

  Context::sourceManager()->addSource(EventSourceRef(
      new SpawnerToDaemon(daemon_path)
  ));
}

void Spawner::initialise() {
  this->registerDefaultSourceManager();
  this->connectDaemon();
}
