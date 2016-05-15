// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <unistd.h>
#include <gtest/gtest.h>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/source/manual.h"
#include "core/event/source/scheduled.h"

#include "core/model/event.h"
#include "core/posix/user.h"
#include "ext/event/manager/epoll.h"

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::ManualSource;
using sf::core::event::ScheduledClosure;
using sf::core::event::ScheduledSource;

using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::EventSourceManagerRef;
using sf::core::model::EventSourceRef;

using sf::core::posix::User;
using sf::ext::event::EpollSourceManager;


class ScheduledSourceTest : public ::testing::Test {
 public:
  ScheduledSource* source;

  ScheduledSourceTest() {
    Static::initialise(new User());
    Context::instance()->initialise(EventSourceManagerRef(
        new EpollSourceManager()
    ));

    // Create an instance for the tests.
    this->source = new ScheduledSource(1, "test");
    Context::sourceManager()->addSource(EventSourceRef(source));
    Context::sourceManager()->addSource(
        EventSourceRef(new ManualSource())
    );
  }

  ~ScheduledSourceTest() {
    Context::destroy();
    Static::destroy();
  }

  EventRef wait(int units) {
    sleep(units);
    return Context::sourceManager()->wait(1);
  }
};


static bool fn_called;
EventRef test_callback(void* closure) {
  fn_called = true;
  if (closure != nullptr) {
    bool* called = reinterpret_cast<bool*>(closure);
    *called = true;
  }
  return EventRef();
}


TEST_F(ScheduledSourceTest, CallsClosure) {
  bool called = false;
  ScheduledClosure closure = {test_callback, &called};
  this->source->registerCallback(0, closure);
  EventRef event = this->wait(2);
  ASSERT_TRUE(called);
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
