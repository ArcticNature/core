// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <fcntl.h>
#include <string>
#include <utility>

#include "core/bin/manager.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/drain/fd.h"
#include "core/event/source/unix.h"
#include "core/exceptions/base.h"

#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/protocols/manager_spawner/ms_message.pb.h"
#include "core/utility/protobuf.h"
#include "core/utility/string.h"


using sf::core::bin::ManagerToSpawner;
using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::FdDrain;
using sf::core::event::UnixClient;
using sf::core::exception::CleanExit;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::protocol::manager_spawner::Message;
using sf::core::utility::MessageIO;
using sf::core::utility::string::toString;


class ManagerToSpawnerFdDrain : public FdDrain {
 public:
  ManagerToSpawnerFdDrain(int fd, std::string id) : FdDrain(fd, id) {}

  void sendAck() {
    Message message;
    message.set_code(Message::Ack);
    MessageIO<Message>::send(this->getFD(), message);
  }
};


EventRef ManagerToSpawner::process(Message message) {
  return EventRef();
}


std::pair<std::string, std::string> ManagerToSpawner::Connect(
    std::string path
) {
  return UnixClient::Connect<ManagerToSpawner, ManagerToSpawnerFdDrain>(
      path, "manager-to-spawner"
  );
}


ManagerToSpawner::ManagerToSpawner(
    int fd, std::string id, std::string drain
) : UnixClient(fd, id, drain) {}

EventRef ManagerToSpawner::parse() {
  if (!this->checkFD()) {
    return EventRef();
  }

  Message message;
  bool valid = MessageIO<Message>::parse(this->getFD(), &message);
  if (!valid) {
    LogInfo info = {{"source-id",  this->id()}};
    ERRORV(
        Context::logger(), "Source ${source-id} received invalid event.",
        info
    );
    return EventRef();
  }

  EventRef event = this->process(message);
  if (event) {
    return event;
  }

  // Failed to recognise the event.
  LogInfo info = {
    {"source-id",  this->id()},
    {"event-code", toString(message.code())},
    {"event-name", Message::Code_Name(message.code())}
  };
  ERRORV(
      Context::logger(),
      "Source ${source-id} received unkown event ${event-name}.", info
  );
  return EventRef();
}
