// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/unix.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/model/logger.h"
#include "core/utility/string.h"


using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::UnixClient;
using sf::core::event::UnixSource;

using sf::core::model::EventRef;
using sf::core::model::EventSource;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::utility::string::toString;


void UnixSource::openSocket() {
  // Create the socket.
  this->socket_fd = Static::posix()->socket(
      AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0
  );

  // Bind to path.
  struct sockaddr_un local;
  local.sun_family = AF_UNIX;
  strncpy(local.sun_path, this->path.c_str(), sizeof(local.sun_path));
  Static::posix()->bind(
      this->socket_fd, (struct sockaddr*)&local, sizeof(sockaddr_un)
  );

  // Enter listen mode.
  Static::posix()->listen(this->socket_fd, this->backlog);

  // Log the opening of the server socket.
  LogInfo info = { {"socket",  this->path} };
  INFOV(
      Context::logger(),
      "Listening for connections on UNIX socket at ${socket}.",
      info
  );
}


UnixSource::UnixSource(std::string path, std::string id) : EventSource(
    "unix-" + id
) {
  this->backlog = 1;
  this->socket_fd = -1;
  this->path = path;
}

UnixSource::~UnixSource() {
  // Close socket.
  if (this->socket_fd != -1) {
    Static::posix()->close(this->socket_fd);
  }

  // And delete file if needed.
  struct stat stat_info;
  if (Static::posix()->stat(this->path.c_str(), &stat_info) == 0) {
    Static::posix()->unlink(this->path.c_str());
  }
}

int UnixSource::getFD() {
  if (this->socket_fd == -1) {
    this->openSocket();
  }
  return this->socket_fd;
}

EventRef UnixSource::parse() {
  // Connect to the client.
  int client_fd = Static::posix()->accept(
      this->socket_fd, nullptr, nullptr,
      SOCK_NONBLOCK | SOCK_CLOEXEC
  );
  std::string client_id = this->id() + "-client-" + toString(client_fd);

  Context::sourceManager()->addSource(
      this->clientSource(client_fd, client_id)
  );

  int drain_fd = Static::posix()->dup(client_fd);
  int flags = Static::posix()->fcntl(client_fd, F_GETFD);
  Static::posix()->fcntl(drain_fd, F_SETFD, flags);
  Static::drains()->add(this->clientDrain(drain_fd, client_id));

  // Done.
  return EventRef();
}


void UnixClient::openSocket() {
  // Create the socket.
  this->socket_fd = Static::posix()->socket(
      AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0
  );

  // Connect to it.
  struct sockaddr_un remote;
  remote.sun_family = AF_UNIX;
  strncpy(remote.sun_path, this->path.c_str(), sizeof(remote.sun_path));

  Static::posix()->connect(
      this->socket_fd, (struct sockaddr*)&remote,
      sizeof(sockaddr_un)
  );

  // Log the opening of the server socket.
  LogInfo info = { {"socket",  this->path} };
  INFOV(
      Context::logger(),
      "Connected to UNIX socket at ${socket}.",
      info
  );
}


UnixClient::UnixClient(std::string path, std::string id) : EventSource(
    "unix-client-" + id
) {
  this->socket_fd = -1;
  this->path = path;
}

UnixClient::~UnixClient() {
  // Close socket.
  if (this->socket_fd != -1) {
    Static::posix()->close(this->socket_fd);
  }

  // And delete file if needed.
  struct stat stat_info;
  if (Static::posix()->stat(this->path.c_str(), &stat_info) == 0) {
    Static::posix()->unlink(this->path.c_str());
  }
}

int UnixClient::getFD() {
  if (this->socket_fd == -1) {
    this->openSocket();
  }
  return this->socket_fd;
}
