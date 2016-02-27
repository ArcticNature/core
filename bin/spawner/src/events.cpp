// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/spawner.h"

#include "core/event/drain/fd.h"
#include "core/event/source/unix.h"

#include "core/model/event.h"


using sf::core::bin::SpawnerToDaemon;

using sf::core::event::FdDrain;
using sf::core::event::UnixClient;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;


SpawnerToDaemon::SpawnerToDaemon(std::string path) : UnixClient(
    path, "spawner-to-daemon"
) {
  int fd = this->getFD();
  // TODO(stefano): Create drain and add it to static context.
}

EventRef SpawnerToDaemon::parse() {
  return EventRef();
}
