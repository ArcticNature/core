// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/daemon.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/drain/fd.h"
#include "core/event/source/fd.h"

#include "core/model/logger.h"
#include "core/protocols/daemon_manager/dm_message.pb.h"
#include "core/utility/protobuf.h"


using sf::core::bin::DaemonToManagerSource;
using sf::core::context::Context;
using sf::core::context::ProxyLogger;
using sf::core::context::Static;

using sf::core::event::FdDrain;
using sf::core::event::FdSource;
using sf::core::event::UnixSource;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::protocol::daemon_manager::Message;
using sf::core::utility::MessageIO;


static ProxyLogger logger("core.bin.daemon.manager");


class ManagerFdDrain : public FdDrain {
 public:
  ManagerFdDrain(int fd, std::string id) : FdDrain(fd, id) {
    // Noop.
  }
};


class ManagerFdSource : public FdSource {
 public:
  ManagerFdSource(int fd, std::string id, EventDrainRef drain) : FdSource(
      fd, id, drain
  ) {
    // Noop.
  }

  EventRef parse() {
    if (!this->checkFD()) {
      return EventRef();
    }
    return EventRef();
  }
};


DaemonToManagerSource::DaemonToManagerSource(std::string path) : UnixSource(
    path, "daemon-to-manager"
) {
  // Noop.
}


EventDrainRef DaemonToManagerSource::clientDrain(int fd, std::string id) {
  EventDrainRef drain(new ManagerFdDrain(fd, id));
  std::string   drain_id = drain->id();

  LogInfo info = {{"drain-id", drain_id}};
  DEBUGV(logger, "Created manager drain ${drain-id}", info);

  Static::options()->setString("manager-drain", drain_id);
  return drain;
}

EventSourceRef DaemonToManagerSource::clientSource(
    int fd, std::string id, EventDrainRef drain
) {
  LogInfo info = {{"source-id", id}};
  DEBUGV(logger, "Creating manager source ${source-id}", info);
  return EventSourceRef(new ManagerFdSource(fd, id, drain));
}
