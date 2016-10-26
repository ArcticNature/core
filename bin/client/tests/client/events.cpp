// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <string>

#include "core/bin/client/events.h"
#include "core/context/client.h"
#include "core/context/static.h"

#include "core/event/drain/null.h"
#include "core/event/testing.h"
#include "core/lifecycle/event.h"

#include "core/model/event.h"
#include "core/posix/user.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_node_info.pb.h"
#include "core/utility/lua/registry.h"
#include "core/utility/protobuf.h"

using sf::core::bin::NodeStatusRequest;
using sf::core::bin::NodeStatusContext;
using sf::core::bin::NSClientContextRef;

using sf::core::context::Client;
using sf::core::context::Static;

using sf::core::event::MockDrain;
using sf::core::event::NullDrain;
using sf::core::lifecycle::EventLifecycle;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::posix::User;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::NodeInfoRequest;
using sf::core::utility::MessageIO;


class NodeStatusRequestTest : public ::testing::Test {
 protected:
  EventDrainRef null;
  EventDrainRef mock;
  MockDrain* mock_drain;
  int test_reference;

 public:
  NodeStatusRequestTest() {
    Static::options()->setString("process-name", "test");
    Static::initialise(new User());
    this->mock_drain = new MockDrain("mock");
    this->mock = EventDrainRef(this->mock_drain);
    this->null = EventDrainRef(new NullDrain());

    Static::drains()->add(this->mock->id(), this->mock);
    Static::drains()->add(this->null->id(), this->null);

    Client::lua().doString("return function()\n  return 2\nend");
    this->test_reference = Client::lua().registry()->referenceTop();
  }

  ~NodeStatusRequestTest() {
    this->mock.reset();
    this->null.reset();
    NodeStatusContext::destroy();
    EventLifecycle::reset();
    Client::destroy();
    Static::destroy();
  }

  EventRef make(EventDrainRef drain) {
    return EventLifecycle::make<NodeStatusRequest>(
        this->test_reference, true, drain
    );
  }
};


TEST_F(NodeStatusRequestTest, EventContextIsCreated) {
  EventRef event = this->make(this->null);
  event->handle();
  NSClientContextRef context = NodeStatusContext::pop(event->correlation());
  ASSERT_EQ(this->test_reference, context->callback_ref);
}

TEST_F(NodeStatusRequestTest, SendsRequest) {
  EventRef event = this->make(this->mock);
  Message  message;
  event->handle();
  bool parsed = MessageIO<Message>::parse(
      this->mock_drain->readFD(), &message
  );
  ASSERT_TRUE(parsed);
  ASSERT_EQ(Message::NodeInfoRequest, message.code());
  ASSERT_TRUE(message.GetExtension(NodeInfoRequest::msg).details());
}

// TODO(stefano): EventContext is deleted on error.
