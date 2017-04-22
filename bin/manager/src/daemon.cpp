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

#include "core/protocols/daemon_manager/dm_message.pb.h"
#include "core/utility/protobuf.h"
#include "core/utility/string.h"


using sf::core::bin::ManagerToDaemon;
using sf::core::context::ProxyLogger;
using sf::core::context::Static;

using sf::core::event::FdDrain;
using sf::core::event::UnixClient;
using sf::core::exception::CleanExit;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::protocol::daemon_manager::Message;
using sf::core::utility::MessageIO;
using sf::core::utility::string::toString;


static ProxyLogger logger("core.bin.manager.daemon");


class ManagerToDaemonFdDrain : public FdDrain {
 public:
  ManagerToDaemonFdDrain(int fd, std::string id) : FdDrain(fd, id) {
    // Noop.
  }
};


EventRef ManagerToDaemon::process(Message message) {
  switch (message.code()) {
    case Message::Shutdown:
      throw CleanExit();
      break;
  }
  return EventRef();
}


std::pair<std::string, std::string> ManagerToDaemon::Connect(
    std::string path
) {
  return UnixClient::Connect<ManagerToDaemon, ManagerToDaemonFdDrain>(
      path, "manager-to-daemon"
  );
}


ManagerToDaemon::ManagerToDaemon(
    int fd, std::string id, EventDrainRef drain
) : UnixClient(fd, id, drain) {
  // Noop.
}

EventRef ManagerToDaemon::parse() {
  if (!this->checkFD()) {
    return EventRef();
  }

  Message message;
  bool valid = MessageIO<Message>::parse(this->fd(), &message);
  if (!valid) {
    LogInfo info = {{"source-id",  this->id()}};
    ERRORV(
        logger, "Source ${source-id} received invalid event.",
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
      logger,
      "Source ${source-id} received unkown event ${event-name}.",
      info
  );
  return EventRef();
}
