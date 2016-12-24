// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/testing.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/drain/fd.h"
#include "core/exceptions/base.h"
#include "core/exceptions/event.h"

#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/utility/string.h"


using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::FdDrain;
using sf::core::event::MockDrain;
using sf::core::event::TestEvent;
using sf::core::event::TestEpollManager;
using sf::core::event::TestFdDrain;
using sf::core::event::TestUnixClient;

using sf::core::exception::ErrNoException;
using sf::core::exception::EventSourceNotFound;
using sf::core::exception::ItemNotFound;

using sf::core::model::Event;
using sf::core::model::EventDrain;
using sf::core::model::EventDrainBufferRef;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::utility::string::toString;


class NullDrain : public EventDrain {
 public:
  NullDrain() : EventDrain("NULL") {
    // Noop.
  }

  int fd() {
    return -1;
  }

  bool flush() {
    return true;
  }
};


MockDrain::MockDrain(std::string id) : EventDrain(id) {
  this->read_fd  = 0;
  this->write_fd = 0;
}

MockDrain::~MockDrain() {
  if (this->read_fd != 0) {
    Static::posix()->close(this->read_fd);
  }
  if (this->write_fd != 0) {
    Static::posix()->close(this->write_fd);
  }
}

int MockDrain::fd() {
  if (this->write_fd == 0) {
    int pipe[2];
    Static::posix()->pipe(pipe, O_NONBLOCK);
    this->read_fd  = pipe[0];
    this->write_fd = pipe[1];
  }
  return this->write_fd;
}

bool MockDrain::flush() {
  if (this->buffer.empty()) {
    return true;
  }

  // Flush the top of the buffer.
  EventDrainBufferRef item = this->buffer[0];
  this->buffer.erase(this->buffer.begin());

  uint32_t size;
  char* data = item->remaining(&size);
  Static::posix()->write(this->fd(), data, size);
  return this->buffer.empty();
}

int MockDrain::readFD() {
  this->fd();  // Ensure drain is open.
  return this->read_fd;
}


TestEvent::TestEvent() : Event(
    "", EventDrainRef(new NullDrain())
) {
  // Noop.
}

TestEvent::TestEvent(
    std::string correlation, EventDrainRef drain
) : Event(correlation, drain) {
  // Noop.
}

void TestEvent::handle() {
  // Noop.
}


TestEpollManager::TestEpollManager() {
  this->epoll_fd = Static::posix()->epoll_create();
}

TestEpollManager::~TestEpollManager() {
  Static::posix()->close(this->epoll_fd, true);
}

void TestEpollManager::add(EventDrainRef drain) {
  // Noop.
}

void TestEpollManager::add(EventSourceRef source) {
  struct epoll_event event;
  int fd = this->fdFor(source);
  event.data.fd = fd;
  event.events  = EPOLLIN | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP;

  Static::posix()->epoll_control(this->epoll_fd, EPOLL_CTL_ADD, fd, &event);

  this->sources.add(source);
}

void TestEpollManager::removeDrain(std::string id) {
  // NOOP.
}

void TestEpollManager::removeSource(std::string id) {
  EventSourceRef source;
  try {
    source = this->sources.get(id);
  } catch(ItemNotFound&) {
    throw EventSourceNotFound(id);
  }

  try {
    int fd = this->fdFor(source);
    Static::posix()->epoll_control(this->epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
  } catch (ErrNoException& ex) {
    if (ex.getCode() != EBADF) {
      throw;
    }
    // Ignore bad file descriptors only.
  }

  this->sources.remove(id);
}

EventRef TestEpollManager::wait(int timeout) {
  struct epoll_event event = {0};
  int code = Static::posix()->epoll_wait(this->epoll_fd, &event, 1, timeout);

  int fd = event.data.fd;
  if (code == 0) {
    DEBUG(Context::Logger(), "Epoll wait timeout");
    return EventRef();
  }

  try {
    return this->sources.get(fd)->fetch();
  } catch (EventSourceNotFound&) {
    LogInfo vars = {{"source", toString(fd)}};
    ERRORV(Context::Logger(), "Unable to find source for FD ${source}.", vars);
    return EventRef();
  }
}


TestFdDrain::TestFdDrain(int fd, std::string id) : FdDrain(fd, id) {
  // Noop.
}


void TestUnixClient::openSocket() {
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
}


TestUnixClient::TestUnixClient(std::string path) : EventSource(
    "unix-client-test"
) {
  this->socket_fd = -1;
  this->path = path;
}

TestUnixClient::~TestUnixClient() {
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

int TestUnixClient::fd() {
  if (this->socket_fd == -1) {
    this->openSocket();
  }
  return this->socket_fd;
}

EventRef TestUnixClient::parse() {
  return EventRef();
}
