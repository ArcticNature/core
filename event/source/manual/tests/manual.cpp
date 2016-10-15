// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/event/source/manual.h"

#include "core/event/testing.h"
#include "core/posix/user.h"

using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::ManualSource;

using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManagerRef;

using sf::core::event::TestEpollManager;
using sf::core::event::TestEvent;
using sf::core::posix::User;


class ManualSourceTest : public ::testing::Test {
 public:
  ManualSource* source;

  ManualSourceTest() {
    Static::initialise(new User());
    Context::Instance()->initialise(LoopManagerRef(new TestEpollManager()));

    // Create an instance for the tests.
    this->source = new ManualSource();
    Context::LoopManager()->add(EventSourceRef(source));
  }

  ~ManualSourceTest() {
    Context::Destroy();
    Static::destroy();
  }
};

TEST_F(ManualSourceTest, NoOp) {
  EventRef event = Context::LoopManager()->wait(1);
  ASSERT_EQ(nullptr, event.get());
}

TEST_F(ManualSourceTest, EnqueueOne) {
  EventRef test1 = EventRef(new TestEvent());
  this->source->enqueueEvent(test1);

  EventRef event = Context::LoopManager()->wait(1);
  ASSERT_EQ(test1.get(), event.get());
}

TEST_F(ManualSourceTest, EnqueueTwo) {
  EventRef test1 = EventRef(new TestEvent());
  EventRef test2 = EventRef(new TestEvent());
  ASSERT_NE(test1.get(), test2.get());
  this->source->enqueueEvent(test1);
  this->source->enqueueEvent(test2);

  EventRef event = Context::LoopManager()->wait(1);
  ASSERT_EQ(test1.get(), event.get());
  event = Context::LoopManager()->wait(1);
  ASSERT_EQ(test2.get(), event.get());
}
