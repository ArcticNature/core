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

#include "core/protocols/test/t_message.pb.h"
#include "core/utility/protobuf.h"
#include "core/utility/string.h"


using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::FdDrain;
using sf::core::event::MockDrain;
using sf::core::event::TestEvent;
using sf::core::event::TestEpollManager;
using sf::core::event::TestFdDrain;
using sf::core::event::TestFdSource;
using sf::core::event::TestUnixClient;

using sf::core::exception::ErrNoException;
using sf::core::exception::EventSourceNotFound;

using sf::core::model::Event;
using sf::core::model::EventDrain;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::protocol::test::Message;
using sf::core::utility::MessageIO;
using sf::core::utility::string::toString;


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

int MockDrain::getFD() {
  if (this->write_fd == 0) {
    int pipe[2];
    Static::posix()->pipe(pipe, O_NONBLOCK);
    this->read_fd  = pipe[0];
    this->write_fd = pipe[1];
  }
  return this->write_fd;
}

void MockDrain::sendAck() {
  // NOOP.
}

int MockDrain::readFD() {
  return this->read_fd;
}


TestEvent::TestEvent() : Event("", "NULL") {}
TestEvent::TestEvent(std::string correlation, std::string drain) : Event(
    correlation, drain
) {
}

void TestEvent::handle() {
}


TestEpollManager::TestEpollManager() {
  this->epoll_fd = Static::posix()->epoll_create();
}

TestEpollManager::~TestEpollManager() {
  Static::posix()->close(this->epoll_fd, true);
}


void TestEpollManager::add(EventSourceRef source) {
  struct epoll_event event;
  int fd = source->getFD();
  event.data.fd = fd;
  event.events  = EPOLLIN | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP;

  Static::posix()->epoll_control(this->epoll_fd, EPOLL_CTL_ADD, fd, &event);

  this->sources[source->id()] = source;
  this->index[fd] = source;
}

void TestEpollManager::remove(std::string id) {
  if (this->sources.find(id) == this->sources.end()) {
    throw EventSourceNotFound(id);
  }

  EventSourceRef source = this->sources.at(id);
  int fd = source->getFD();

  try {
    Static::posix()->epoll_control(this->epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
  } catch (ErrNoException& ex) {
    if (ex.getCode() != EBADF) {
      throw;
    }
    // Ignore bad file descriptors only.
  }

  this->index.erase(fd);
  this->sources.erase(id);
}

EventRef TestEpollManager::wait(int timeout) {
  struct epoll_event event = {0};
  int code = Static::posix()->epoll_wait(this->epoll_fd, &event, 1, timeout);

  int fd = event.data.fd;
  if (code == 0) {
    DEBUG(Context::logger(), "Epoll wait timeout");
    return EventRef();
  }

  if (this->index.find(fd) == this->index.end()) {
    LogInfo vars = {{"source", toString(fd)}};
    ERRORV(Context::logger(), "Unable to find source for FD ${source}.", vars);
    return EventRef();
  }

  return this->index[fd]->parse();
}


TestFdDrain::TestFdDrain(int fd, std::string id) : FdDrain(fd, id) {}
void TestFdDrain::sendAck() {}


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


TestFdSource::TestFdSource(int fd, std::string id) : EventSource("fd-" + id) {
  this->fd = fd;
}

TestFdSource::~TestFdSource() {
  Static::posix()->shutdown(this->fd, SHUT_RD);
  Static::posix()->close(this->fd);
}

int TestFdSource::getFD() {
  return this->fd;
}

EventRef TestFdSource::parse() {
  Message message;
  bool valid = MessageIO<Message>::parse(this->fd, &message);
  if (valid && message.code() == Message::Test) {
    return EventRef(new TestEvent("abc", "NULL"));
  }
  return EventRef();
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

int TestUnixClient::getFD() {
  if (this->socket_fd == -1) {
    this->openSocket();
  }
  return this->socket_fd;
}

EventRef TestUnixClient::parse() {
  return EventRef();
}
