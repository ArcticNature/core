// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <string>

#include "core/context/static.h"
#include "core/lifecycle/event.h"

#include "core/event/testing.h"
#include "core/model/cli-parser.h"

using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;

using sf::core::context::Static;
using sf::core::lifecycle::EventLifecycle;
using sf::core::model::EventRef;

using sf::core::event::TestEvent;
using sf::core::model::CLIParser;


class NullParser : public CLIParser {
 protected:
  void parseLogic(int* argc, char*** argv) {
    return;
  }
};


class TestEvent2 : public TestEvent {
 public:
  TestEvent2() : TestEvent() {}
  TestEvent2(std::string cor, std::string drain) : TestEvent(cor, drain) {}
};


class InitCallHandler : public BaseLifecycleHandler {
 public:
  static uint64_t call_count;
  void handle(std::string, BaseLifecycleArg*) {
    InitCallHandler::call_count++;
  };
};
uint64_t InitCallHandler::call_count = 0;
LifecycleStaticOn("event::initialise", InitCallHandler);


class EventLifecycleTest : public ::testing::Test {
 public:
  EventLifecycleTest() {
    Static::options()->setString("process-name", "node");
    Static::parser(new NullParser());
  }

  ~EventLifecycleTest() {
    EventLifecycle::reset();
    Static::destroy();
  }
};


TEST_F(EventLifecycleTest, AddsId) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#0", event->id());
}

TEST_F(EventLifecycleTest, AddsIdSequencially) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#0", event->id());

  EventRef event2(new TestEvent());
  EventLifecycle::Init(event2);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#1", event2->id());
}

TEST_F(EventLifecycleTest, CorrelationIsIdIfNotSet) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#0", event->id());
  ASSERT_EQ("node!N2sf4core5event9TestEventE#0", event->correlation());
}

TEST_F(EventLifecycleTest, CorrelationIsPreserved) {
  EventRef event(new TestEvent("set-outside", "NULL"));
  EventLifecycle::Init(event);
  ASSERT_EQ("set-outside", event->correlation());
}

TEST_F(EventLifecycleTest, IdsAreSetOnce) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#0", event->id());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#0", event->id());
}

TEST_F(EventLifecycleTest, IdIsBasedOnClass) {
  EventRef event(new TestEvent2());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!10TestEvent2#0", event->id());
}

TEST_F(EventLifecycleTest, InitIsCalled) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_LT(0, InitCallHandler::call_count);
}
