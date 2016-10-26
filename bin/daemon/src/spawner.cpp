// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/daemon.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/drain/fd.h"
#include "core/event/source/fd.h"

#include "core/model/logger.h"
#include "core/protocols/daemon_spanwer/ds_message.pb.h"
#include "core/utility/protobuf.h"


using sf::core::bin::DaemonToSpawnerSource;
using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::FdDrain;
using sf::core::event::FdSource;
using sf::core::event::UnixSource;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::protocol::daemon_spanwer::Message;
using sf::core::utility::MessageIO;


class SpawnerFdDrain : public FdDrain {
 public:
  SpawnerFdDrain(int fd, std::string id) : FdDrain(fd, id) {
    // Noop.
  }
};


class SpawnerFdSource : public FdSource {
 public:
  SpawnerFdSource(int fd, std::string id, EventDrainRef drain) : FdSource(
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


DaemonToSpawnerSource::DaemonToSpawnerSource(std::string path) : UnixSource(
    path, "daemon-to-spawner"
) {}


EventDrainRef DaemonToSpawnerSource::clientDrain(int fd, std::string id) {
  EventDrainRef drain(new SpawnerFdDrain(fd, id));
  std::string   drain_id = drain->id();

  LogInfo info = {{"drain-id", drain_id}};
  DEBUGV(Context::Logger(), "Created spawner drain ${drain-id}", info);

  Static::options()->setString("spawner-drain", drain_id);
  return drain;
}

EventSourceRef DaemonToSpawnerSource::clientSource(
    int fd, std::string id, EventDrainRef drain
) {
  LogInfo info = {{"source-id", id}};
  DEBUGV(Context::Logger(), "Creating spawner source ${source-id}", info);
  return EventSourceRef(new SpawnerFdSource(fd, id, drain));
}
