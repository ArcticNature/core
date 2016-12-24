// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <fcntl.h>
#include <string.h>
#include <gtest/gtest.h>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/event.h"

#include "core/event/source/connected.h"
#include "core/event/source/unix.h"
#include "core/model/event.h"

#include "core/event/testing.h"
#include "core/posix/user.h"
#include "core/utility/net.h"

#define SOCKET_PATH "./core.event.source.connected.test"


using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::exception::EventDrainNotFound;
using sf::core::exception::EventSourceNotFound;

using sf::core::event::ConnectedSource;
using sf::core::event::UnixSource;

using sf::core::model::EventDrain;
using sf::core::model::EventDrainBuffer;
using sf::core::model::EventDrainBufferRef;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManagerRef;

using sf::core::event::TestEpollManager;
using sf::core::event::TestEvent;
using sf::core::event::TestFdDrain;
using sf::core::event::TestUnixClient;

using sf::core::posix::User;
using sf::core::utility::FileDescriptorState;


class TestSource : public ConnectedSource {
 protected:
  int _fd;

  void onclosed() {
    ConnectedSource::onclose();
    this->_fd = -1;
  }

 public:
  TestSource(
      int fd, std::string id, EventDrainRef drain
  ) : ConnectedSource(id, drain) {
    this->_fd = fd;
  }

  ~TestSource() {
    this->safeCleanUp(false);
  }

  void disconnect() {
    Static::posix()->close(this->_fd);
  }

  int fd() {
    return this->_fd;
  }

  EventRef parse() {
    // Ensure the FD is usable.
    if (!this->checkFD()) {
      return EventRef();
    }

    // Read data from the connection.
    char buffer[512];
    Static::posix()->read(this->fd(), buffer, 512);
    TestEvent* event = new TestEvent();
    event->payload = std::string(buffer);
    return EventRef(event);
  }
};


class ConnectedUnixSource : public UnixSource {
 protected:
   EventDrainRef  drain;
   EventSourceRef source;
   int source_fd;

   EventDrainRef clientDrain(int fd, std::string id) {
     this->drain = EventDrainRef(new TestFdDrain(fd, id));
     return this->drain;
   }

   EventSourceRef clientSource(int fd, std::string id, EventDrainRef drain) {
     this->source = EventSourceRef(new TestSource(fd, id, drain));
     this->source_fd = fd;
     return this->source;
   }

 public:
   ConnectedUnixSource(std::string path) : UnixSource(path, "test") {
     // Noop.
   }

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
  EventSourceRef client_source;
  EventDrainRef  server_drain;
  EventSourceRef server_source;

 public:
  ConnectedSourceTest() {
    Static::initialise(new User());
    Context::Instance()->initialise(LoopManagerRef(new TestEpollManager()));

    try {
      // Start listening for connections.
      ConnectedUnixSource* listen = new ConnectedUnixSource(SOCKET_PATH);
      Context::LoopManager()->add(EventSourceRef(listen));

      // Connect to the server.
      EventSourceRef client(new TestUnixClient(SOCKET_PATH));
      Context::LoopManager()->add(client);
      Context::LoopManager()->wait(1);

      // Keep references to useful bits.
      this->client_source = client;
      this->server_drain = listen->getClientDrain();
      this->server_source = listen->getClientSource();
    } catch (...) {
      Static::destroy();
      throw;
    }
  }

  ~ConnectedSourceTest() {
    this->client_source.reset();
    this->server_drain.reset();
    this->server_source.reset();
    Context::Destroy();
    Static::destroy();
  }

  void closeClient() {
    std::string id = this->client_source->id();
    this->client_source.reset();
    Context::LoopManager()->removeSource(id);
  }
};


TEST_F(ConnectedSourceTest, SourceIsRemovedFromManager) {
  LoopManagerRef manager = Context::LoopManager();
  this->closeClient();
  EventRef event = manager->wait(1);

  ASSERT_EQ(nullptr, event.get());
  ASSERT_THROW(
      manager->removeSource(this->server_source->id()),
      EventSourceNotFound
  );
}

TEST_F(ConnectedSourceTest, DrainIsRemovedFromManager) {
  std::string drain_id = this->server_drain->id();
  this->closeClient();
  EventRef event = Context::LoopManager()->wait(1);

  ASSERT_EQ(nullptr, event.get());
  ASSERT_THROW(Static::drains()->remove(drain_id), EventDrainNotFound);
}
