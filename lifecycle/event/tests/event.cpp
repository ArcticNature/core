// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <string>

#include "core/event/testing.h"
#include "core/lifecycle/event.h"

using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;

using sf::core::lifecycle::EventLifecycle;
using sf::core::model::EventRef;

using sf::core::event::TestEvent;


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


TEST(EventLifecycleId, AddsId) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#0", event->id());
}

TEST(EventLifecycleId, IdIsBasedOnClass) {
  EventRef event(new TestEvent2());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!10TestEvent2#0", event->id());
}

TEST(EventLifecycleId, AddsIdSequencially) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#1", event->id());
}

TEST(EventLifecycleId, IdsAreSetOnce) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#2", event->id());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#2", event->id());
}

TEST(EventLifecycle, InitIsCalled) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ(6, InitCallHandler::call_count);
}

TEST(EventLifecycleCorrelation, IsIdIfNotSet) {
  EventRef event(new TestEvent());
  EventLifecycle::Init(event);
  ASSERT_EQ("node!N2sf4core5event9TestEventE#5", event->id());
  ASSERT_EQ("node!N2sf4core5event9TestEventE#5", event->correlation());
}

TEST(EventLifecycleCorrelation, IsPreserved) {
  EventRef event(new TestEvent("set-outside", "NULL"));
  EventLifecycle::Init(event);
  ASSERT_EQ("set-outside", event->correlation());
}
