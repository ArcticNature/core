// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_CLIENT_UNIX_INC_H_
#define CORE_EVENT_CLIENT_UNIX_INC_H_

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <string>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/model/event.h"
#include "core/utility/string.h"

namespace sf {
namespace core {
namespace event {

  template<class SourceClass, class DrainClass>
  std::string UnixClient::Connect(std::string socket, std::string id) {
    // Debug log the path to the socket.
    sf::core::model::LogInfo info = { {"socket", socket} };
    DEBUGV(
        sf::core::context::Context::logger(),
        "Establishing UNIX socket at ${socket}.",
        info
    );

    // Connect to the server.
    int source_fd = sf::core::context::Static::posix()->socket(
        AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0
    );
    std::string fd_id = id + "-" + sf::core::utility::string::toString(
        source_fd
    );

    // Connect to it.
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strncpy(remote.sun_path, socket.c_str(), sizeof(remote.sun_path));
    sf::core::context::Static::posix()->connect(
        source_fd, (struct sockaddr*)&remote,
        sizeof(sockaddr_un)
    );

    // Duplicate the socket for the drain.
    int drain_fd = sf::core::context::Static::posix()->dup(source_fd);
    int flags = sf::core::context::Static::posix()->fcntl(source_fd, F_GETFD);
    sf::core::context::Static::posix()->fcntl(drain_fd, F_SETFD, flags);

    // Create the drain.
    sf::core::model::EventDrainRef drain(new DrainClass(drain_fd, fd_id));

    // Create the source.
    sf::core::model::EventSourceRef source(new SourceClass(
          source_fd, fd_id, drain->id()
    ));

    // Register in event managers.
    sf::core::context::Static::drains()->add(drain);
    sf::core::context::Context::sourceManager()->addSource(source);

    // Log connection established.
    info = {
      {"drain-id",  drain->id()},
      {"socket",    socket},
      {"source-id", source->id()}
    };
    INFOV(
        sf::core::context::Context::logger(),
        "Connected to UNIX socket at ${socket}.",
        info
    );
    return drain->id();
  }

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_CLIENT_UNIX_INC_H_
