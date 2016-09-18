// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <fcntl.h>

#include <gtest/gtest.h>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/event.h"

#include "core/event/source/connected.h"
#include "core/event/source/unix.h"
#include "core/model/event.h"

#include "core/event/testing.h"
#include "core/posix/user.h"

#define SOCKET_PATH "./connected.source.test"


using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::exception::EventDrainNotFound;
using sf::core::exception::EventSourceNotFound;

using sf::core::event::ConnectedSource;
using sf::core::event::FileDescriptorState;
using sf::core::event::UnixSource;

using sf::core::model::EventDrain;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceManagerRef;
using sf::core::model::EventSourceRef;

using sf::core::event::TestEpollManager;
using sf::core::event::TestEvent;
using sf::core::event::TestFdDrain;
using sf::core::event::TestUnixClient;

using sf::core::posix::User;


class TestSource : public ConnectedSource {
 protected:
  int fd;

  void onclosed() {
    ConnectedSource::onclose();
    this->fd = -1;
  }

 public:
  TestSource(
      int fd, std::string id, std::string drain
  ) : ConnectedSource(id, drain) {
    this->fd = fd;
  }

  ~TestSource() {
    this->safeCleanUp(false);
  }

  void disconnect() {
    Static::posix()->close(this->fd);
  }

  int getFD() {
    return fd;
  }

  EventRef parse() {
    // Ensure the FD is usable.
    if (!this->checkFD()) {
      return EventRef();
    }
    return EventRef(new TestEvent());
  }
};


class ConnectedUnixSource : public UnixSource {
 protected:
   EventDrainRef  drain;
   EventSourceRef source;

   EventDrainRef clientDrain(int fd, std::string id) {
     this->drain = EventDrainRef(new TestFdDrain(fd, id));
     return this->drain;
   }

   EventSourceRef clientSource(int fd, std::string id, std::string drain_id) {
     this->source = EventSourceRef(new TestSource(fd, id, drain_id));
     return this->source;
   }

 public:
   ConnectedUnixSource(std::string path) : UnixSource(path, "test") {}

   EventDrainRef getClientDrain() {
     EXPECT_NE(nullptr, this->drain.get());
     return this->drain;
   }

   EventSourceRef getClientSource() {
     EXPECT_NE(nullptr, this->source.get());
     return this->source;
   }
};


class ConnectedSourceTest : public ::testing::Test {
 protected:
  EventSourceRef client;
  EventSourceRef server;
  EventDrainRef  server_drain;

  int client_fd;
  int server_fd;

 public:
  ConnectedSourceTest() {
    Static::initialise(new User());
    Context::instance()->initialise(EventSourceManagerRef(
      new TestEpollManager()
    ));

    // Start listening for connections.
    ConnectedUnixSource* listen = new ConnectedUnixSource(SOCKET_PATH);
    Context::sourceManager()->addSource(EventSourceRef(listen));

    // Connect to the server.
    EventSourceRef client(new TestUnixClient(SOCKET_PATH));
    Context::sourceManager()->addSource(client);
    Context::sourceManager()->wait(1);

    // Keep references to useful bits.
    this->server    = listen->getClientSource();
    this->server_fd = this->server->getFD();
    this->client    = client;
    this->client_fd = client->getFD();
    this->server_drain = listen->getClientDrain();
  }

  ~ConnectedSourceTest() {
    this->client = EventSourceRef();
    this->server = EventSourceRef();
    this->server_drain = EventDrainRef();
    Context::destroy();
    Static::destroy();
  }

  void closeClient() {
    std::string id  = this->client->id();
    this->client    = EventSourceRef();
    this->client_fd = -1;
    Context::sourceManager()->removeSource(id);
  }
};


TEST_F(ConnectedSourceTest, CheckDoesNotReadStream) {
  char buffer[] = "hello";
  Static::posix()->write(this->client_fd, buffer, 5);
  FileDescriptorState state = ConnectedSource::checkFdState(this->server_fd);

  buffer[0] = '\0';
  buffer[1] = '\0';
  buffer[2] = '\0';
  buffer[3] = '\0';
  buffer[4] = '\0';
  Static::posix()->read(this->server_fd, buffer, 5);

  ASSERT_EQ(FileDescriptorState::OK, state);
  ASSERT_STREQ("hello", buffer);
}

TEST_F(ConnectedSourceTest, DetectClosed) {
  this->closeClient();
  FileDescriptorState state = ConnectedSource::checkFdState(this->server_fd);
  ASSERT_EQ(FileDescriptorState::CLOSED, state);
}

TEST_F(ConnectedSourceTest, DetectOpen) {
  FileDescriptorState state = ConnectedSource::checkFdState(this->server_fd);
  ASSERT_EQ(FileDescriptorState::OK, state);
}


TEST_F(ConnectedSourceTest, SourceIsRemovedFromManager) {
  EventSourceManagerRef manager = Context::sourceManager();
  this->closeClient();
  EventRef event = manager->wait(1);

  ASSERT_EQ(nullptr, event.get());
  ASSERT_THROW(manager->removeSource(this->server->id()), EventSourceNotFound);
}

TEST_F(ConnectedSourceTest, DrainIsRemovedFromManager) {
  std::string drain_id = this->server_drain->id();
  this->closeClient();
  EventRef event = Context::sourceManager()->wait(1);

  ASSERT_EQ(nullptr, event.get());
  ASSERT_THROW(Static::drains()->remove(drain_id), EventDrainNotFound);
}
