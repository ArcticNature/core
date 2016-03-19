// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <string>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/client/unix.h"
#include "core/event/source/unix.h"
#include "core/event/testing.h"

#include "core/model/event.h"
#include "core/posix/user.h"

#include "core/protocols/test/t_message.pb.h"
#include "core/utility/protobuf.h"

#include "ext/event/manager/epoll.h"

#define SOCKET_PATH "core.event.client.unix.socket"


using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::TestEvent;
using sf::core::event::TestFdDrain;
using sf::core::event::TestFdSource;
using sf::core::event::UnixClient;
using sf::core::event::UnixSource;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceManagerRef;
using sf::core::model::EventSourceRef;
using sf::core::posix::User;

using sf::core::protocol::test::Message;
using sf::core::utility::MessageIO;

using sf::ext::event::EpollSourceManager;


class TestUnixClient : public UnixClient {
 public:
  TestUnixClient(int fd, std::string id, std::string drain_id) : UnixClient(
      fd, id, drain_id
  ) {}

  EventRef parse() {
    return EventRef(new TestEvent("def", "NULL"));
  }
};


class TestUnixServer : public UnixSource {
 protected:
  EventDrainRef clientDrain(int fd, std::string id) {
    EventDrainRef drain(new TestFdDrain(fd, id));
    TestUnixServer::server_side_drain = drain;
    return drain;
  }

  EventSourceRef clientSource(int fd, std::string id, std::string drain_id) {
    TestFdSource* source = new TestFdSource(fd, id);
    return EventSourceRef(source);
  }

 public:
  static EventDrainRef server_side_drain;

  TestUnixServer(std::string socket, std::string id) : UnixSource(
      socket, id
  ) {}
};

EventDrainRef TestUnixServer::server_side_drain;


class UnixClientTest : public ::testing::Test {
 protected:
   std::string connect() {
     return UnixClient::Connect<TestUnixClient, TestFdDrain>(
         SOCKET_PATH, "test"
     );
   }

   bool sendTestMessage(EventDrainRef drain) {
     Message message;
     message.set_code(Message::Test);
     return MessageIO<Message>::send(drain->getFD(), message);
   }

 public:
  UnixClientTest() {
    // Prepare context.
    Static::initialise(new User());
    Context::instance()->initialise(
        EventSourceManagerRef(new EpollSourceManager())
    );

    // Create a test "server" with a UnixSource.
    EventSourceRef server(new TestUnixServer(SOCKET_PATH, "test"));
    Context::sourceManager()->addSource(server);
  }
  ~UnixClientTest() {
    TestUnixServer::server_side_drain = EventDrainRef();
    Context::destroy();
    Static::destroy();
  }
};


TEST_F(UnixClientTest, ConnectsRegistersDrain) {
  this->connect();
  EventDrainRef drain = Static::drains()->get("test-5-fd");
  ASSERT_NE(nullptr, drain.get());
}

TEST_F(UnixClientTest, ConnectsRegistersSource) {
  this->connect();
  Context::sourceManager()->removeSource("test-5-unix-client-fd");
}

TEST_F(UnixClientTest, SendsAMessage) {
  // Establish the connection.
  std::string drain_id = this->connect();
  Context::sourceManager()->wait(1);
  EventDrainRef drain = Static::drains()->get(drain_id);

  // Send message over the drain.
  bool sent = this->sendTestMessage(drain);
  ASSERT_TRUE(sent);

  // Server receives the message.
  EventRef event = Context::sourceManager()->wait(1);
  ASSERT_NE(nullptr, event.get());
  ASSERT_EQ("abc", event->correlation());
}

TEST_F(UnixClientTest, RecievesAMessage) {
  // Establish the connection.
  this->connect();
  Context::sourceManager()->wait(1);
  EventDrainRef drain = TestUnixServer::server_side_drain;
  ASSERT_NE(nullptr, drain.get());

  // Send message over the drain.
  bool sent = this->sendTestMessage(drain);
  ASSERT_TRUE(sent);

  // Server receives the message.
  EventRef event = Context::sourceManager()->wait(1);
  ASSERT_NE(nullptr, event.get());
  ASSERT_EQ("def", event->correlation());
}