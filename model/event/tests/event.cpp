// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/event.h"
#include "core/model/event.h"

using sf::core::exception::EventDrainNotFound;

using sf::core::model::Event;
using sf::core::model::EventDrain;
using sf::core::model::EventDrainManager;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSource;


class TestEvent : public Event {
 public:
  TestEvent(std::string correlation, std::string drain) : Event(
      correlation, drain
  ) {}
  void handle() {}
};


class TestDrain : public EventDrain {
 public:
  TestDrain(std::string drain) : EventDrain(drain) {}
  void send() {}
};


class TestSource : public EventSource {
 public:
  TestSource(std::string id) : EventSource(id) {}
  int getFD() {
    return 1;
  }
  EventRef parse() {
    return EventRef(nullptr);
  }
};


TEST(EventDrain, id) {
  TestDrain drain("id");
  ASSERT_EQ("id", drain.id());
}

TEST(EventDrainManager, add) {
  EventDrainManager manager;
  EventDrainRef drain(new TestDrain("drain"));

  manager.add(drain);
  ASSERT_EQ(drain, manager.get("drain"));
}

TEST(EventDrainManager, get) {
  EventDrainManager manager;
  EventDrainRef drain1(new TestDrain("drain1"));
  EventDrainRef drain2(new TestDrain("drain2"));

  manager.add(drain1);
  manager.add(drain2);
  ASSERT_EQ(drain1, manager.get("drain1"));
  ASSERT_EQ(drain2, manager.get("drain2"));
}

TEST(EventDrainManager, getFails) {
  EventDrainManager manager;
  EXPECT_THROW(manager.get("drain"), EventDrainNotFound);
}

TEST(EventDrainManager, getNull) {
  EventDrainManager manager;
  EventDrainRef drain = manager.get("NULL");
  ASSERT_EQ("NULL", drain->id());
}

TEST(EventDrainManager, remove) {
  EventDrainManager manager;
  EventDrainRef drain1(new TestDrain("drain1"));
  EventDrainRef drain2(new TestDrain("drain2"));

  manager.add(drain1);
  manager.add(drain2);
  manager.remove("drain1");

  EXPECT_THROW(manager.get("drain1"), EventDrainNotFound);
  ASSERT_EQ(drain2, manager.get("drain2"));
}

TEST(EventSource, id) {
  TestSource drain("id");
  ASSERT_EQ("id", drain.id());
}
