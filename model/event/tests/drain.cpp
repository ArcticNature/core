// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <gtest/gtest.h>

#include "core/interface/lifecycle.h"
#include "core/model/event.h"

#include "./fixtures.h"


using sf::core::interface::Lifecycle;
using sf::core::interface::LifecycleHandlerRef;

using sf::core::lifecycle::EVENT_DRAIN_ENQUEUE;
using sf::core::lifecycle::DrainEnqueueArg;
using sf::core::lifecycle::DrainEnqueueHandler;

using sf::core::model::EventDrain;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;

using sf::testing::FailEvent;
using sf::testing::NoopEvent;
using sf::testing::TestDrain;


class TestDrainEnqueued : public DrainEnqueueHandler {
 public:
   std::string drain;
   void handle(std::string event, DrainEnqueueArg* argument) {
     this->drain = argument->drain();
   }
};


TEST(EventDrain, Id) {
  TestDrain drain;
  ASSERT_EQ("test-drain", drain.id());
}

TEST(EventDrain, ReThrows) {
  TestDrain drain;
  std::exception_ptr ex;

  try {
    throw std::exception();
  } catch (std::exception&) {
    ex = std::current_exception();
  }

  ASSERT_THROW(drain.rescue(ex), std::exception);
}

TEST(EventDrain, Enqueue) {
  TestDrain drain;
  drain.enqueue("ABC");
  drain.enqueue("DEF");
  ASSERT_EQ(2, drain.length());
}

TEST(EventDrain, TriggerEnqueue) {
  EventDrainRef drain(new TestDrain());
  TestDrainEnqueued* handler = new TestDrainEnqueued();
  LifecycleHandlerRef h(handler);
  Lifecycle::reset();
  Lifecycle::on(EVENT_DRAIN_ENQUEUE, h);
  drain->enqueue("ABC");
  ASSERT_EQ(drain->id(), handler->drain);
}
