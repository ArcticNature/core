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


using sf::core::bin::DaemonSpawnerSource;
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
  SpawnerFdDrain(int fd, std::string id) : FdDrain(fd, id) {}

  void sendAck() {
    Message message;
    message.set_code(Message::Ack);
    MessageIO<Message>::send(this->fd, message);
  }
};


class SpawnerFdSource : public FdSource {
 public:
  SpawnerFdSource(int fd, std::string id) : FdSource(fd, id) {}

  EventRef parse() {
    return EventRef();
  }
};


DaemonSpawnerSource::DaemonSpawnerSource(std::string path) : UnixSource(
    path, "daemon-to-spawner"
) {}


EventDrainRef DaemonSpawnerSource::clientDrain(int fd, std::string id) {
  LogInfo info = {{"drain-id", id}};
  DEBUGV(Context::logger(), "Create spawner drain ${drain-id}", info);

  EventDrainRef drain(new SpawnerFdDrain(fd, id));
  Static::options()->setString("spawner-drain", id);
  return drain;
}

EventSourceRef DaemonSpawnerSource::clientSource(int fd, std::string id) {
  LogInfo info = {{"source-id", id}};
  DEBUGV(Context::logger(), "Create spawner source ${source-id}", info);
  return EventSourceRef(new SpawnerFdSource(fd, id));
}
