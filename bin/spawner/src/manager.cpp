// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/spawner.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/drain/fd.h"
#include "core/event/source/fd.h"

#include "core/model/logger.h"
#include "core/protocols/manager_spawner/ms_message.pb.h"
#include "core/utility/protobuf.h"


using sf::core::bin::SpawnerToManagerSource;
using sf::core::context::ProxyLogger;
using sf::core::context::Static;

using sf::core::event::FdDrain;
using sf::core::event::FdSource;
using sf::core::event::UnixSource;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::protocol::manager_spawner::Message;
using sf::core::utility::MessageIO;


static ProxyLogger logger("core.bin.spawner.manager");


class ManagerFdDrain : public FdDrain {
 public:
  ManagerFdDrain(int fd, std::string id) : FdDrain(fd, id) {
    // Noop.
  }
};


class ManagerFdSource : public FdSource {
 public:
  ManagerFdSource(
      int fd, std::string id, EventDrainRef drain
  ) : FdSource(fd, id, drain) {
    // Noop.
  }

  EventRef parse() {
    if (!this->checkFD()) {
      return EventRef();
    }

    // TODO(stefano): Parse message from manager.
    return EventRef();
  }
};


SpawnerToManagerSource::SpawnerToManagerSource(std::string path) : UnixSource(
    path, "spawner-to-manager"
) {}


EventDrainRef SpawnerToManagerSource::clientDrain(int fd, std::string id) {
  EventDrainRef drain(new ManagerFdDrain(fd, id));
  std::string   drain_id = drain->id();

  LogInfo info = {{"drain-id", drain_id}};
  DEBUGV(logger, "Created manager drain ${drain-id}", info);

  Static::options()->setString("manager-drain", drain_id);
  return drain;
}

EventSourceRef SpawnerToManagerSource::clientSource(
    int fd, std::string id, EventDrainRef drain
) {
  LogInfo info = {{"source-id", id}};
  DEBUGV(logger, "Creating manager source ${source-id}", info);
  return EventSourceRef(new ManagerFdSource(fd, id, drain));
}
