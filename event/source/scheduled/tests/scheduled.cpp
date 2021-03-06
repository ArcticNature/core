// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <unistd.h>
#include <gtest/gtest.h>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"

#include "core/event/source/manual.h"
#include "core/event/source/scheduled.h"

#include "core/model/event.h"

#include "core/event/testing.h"
#include "core/posix/user.h"


using poolqueue::Promise;

using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::exception::ItemNotFound;

using sf::core::event::ManualSource;
using sf::core::event::ScheduledClosure;
using sf::core::event::ScheduledClosureList;
using sf::core::event::ScheduledSource;
using sf::core::event::TickPromises;

using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManagerRef;

using sf::core::event::TestEpollManager;
using sf::core::posix::User;


class ScheduledSourceTest : public ::testing::Test {
 public:
  ScheduledSource* source;

  ScheduledSourceTest() {
    Static::initialise(new User());
    Context::Instance()->initialise(LoopManagerRef(new TestEpollManager()));

    // Create an instance for the tests.
    this->source = new ScheduledSource(1);
    Context::LoopManager()->add(EventSourceRef(source));
    Context::LoopManager()->add(EventSourceRef(new ManualSource()));
  }

  ~ScheduledSourceTest() {
    Context::Destroy();
    Static::destroy();
  }

  EventRef wait(int units) {
    sleep(units);
    return Context::LoopManager()->wait(1);
  }
};


static bool fn_called;

void delete_bool(void* block) {
  bool* boolean = reinterpret_cast<bool*>(block);
  delete boolean;
}

EventRef test_callback(std::shared_ptr<void> closure) {
  fn_called = true;
  if (closure) {
    bool* called = reinterpret_cast<bool*>(closure.get());
    *called = true;
  }
  return EventRef();
}


TEST_F(ScheduledSourceTest, CallsClosure) {
  ScheduledClosure callback;
  std::shared_ptr<void> closure(new bool);
  bool* called = reinterpret_cast<bool*>(closure.get());

  *called = false;
  callback.callback = test_callback;
  callback.closure = closure;

  this->source->registerCallback(0, callback);
  EventRef event = this->wait(2);
  ASSERT_TRUE(*called);
  ASSERT_EQ(nullptr, event.get());
}

TEST_F(ScheduledSourceTest, CallsFunction) {
  fn_called = false;
  this->source->registerCallback(0, test_callback);
  EventRef event = this->wait(2);
  ASSERT_TRUE(fn_called);
  ASSERT_EQ(nullptr, event.get());
}

TEST_F(ScheduledSourceTest, Waits) {
  fn_called = false;
  this->source->registerCallback(1, test_callback);

  // Tick twice
  this->wait(2);
  EventRef event = this->wait(2);

  ASSERT_TRUE(fn_called);
  ASSERT_EQ(nullptr, event.get());
}

TEST_F(ScheduledSourceTest, ListKeptCallbacks) {
  ScheduledClosure callback;
  std::vector<ScheduledClosureList::ScoredValue> callbacks;

  callback.callback = test_callback;
  callback.keep_on_reconfigure = true;
  this->source->registerCallback(1, test_callback);
  this->source->registerCallback(3, callback);
  this->source->registerCallback(4, test_callback);
  this->source->registerCallback(7, callback);
  this->source->registerCallback(7, callback);
  callbacks = this->source->keepCallbacks();

  ASSERT_EQ(3, callbacks.size());
  ASSERT_EQ(3, callbacks[0].score);
  ASSERT_EQ(7, callbacks[1].score);
  ASSERT_EQ(7, callbacks[2].score);
  ASSERT_EQ(&test_callback, callbacks[0].value.callback);
  ASSERT_EQ(&test_callback, callbacks[1].value.callback);
  ASSERT_EQ(&test_callback, callbacks[2].value.callback);
}

TEST_F(ScheduledSourceTest, TickPromises) {
  bool killed = false;
  Promise promise;
  promise.except([&killed](const std::exception_ptr& e) {
      killed = true;
      return nullptr;
  });

  // Add a promise with a TTL of 1.
  Static::promises.keep("p", promise, 1);

  // Wait for tick and then check manual source.
  this->wait(1);
  EventRef event = this->wait(0);
  ASSERT_NE(nullptr, event.get());

  // Handle ticker and check promise is gone.
  event->handle();
  ASSERT_THROW(Static::promises.pop("p"), ItemNotFound);
  ASSERT_TRUE(killed);
}
