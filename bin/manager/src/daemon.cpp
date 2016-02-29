// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <fcntl.h>
#include <string>

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
using sf::core::context::Context;
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


class ManagerToDaemonFdDrain : public FdDrain {
 public:
  explicit ManagerToDaemonFdDrain(int fd) : FdDrain(
      fd, "manager-to-daemon" + toString(fd)
  ) {}

  void sendAck() {
    Message message;
    message.set_code(Message::Ack);
    MessageIO<Message>::send(this->getFD(), message);
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

ManagerToDaemon::ManagerToDaemon(std::string path) : UnixClient(
    path, "manager-to-daemon"
) {
  // Connect to server.
  int client_fd = this->getFD();

  // Create a file descriptor for the drain.
  int drain_fd = Static::posix()->dup(client_fd);
  int flags = Static::posix()->fcntl(client_fd, F_GETFD);
  Static::posix()->fcntl(drain_fd, F_SETFD, flags);

  // Create drain and add it to static context.
  EventDrainRef drain(new ManagerToDaemonFdDrain(drain_fd));
  Static::drains()->add(drain);
}

EventRef ManagerToDaemon::parse() {
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
