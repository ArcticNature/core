// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/event.h"
#include "core/model/event.h"

using sf::core::exception::SfException;
using sf::core::exception::EventDrainNotFound;
using sf::core::exception::EventSourceNotFound;
using sf::core::exception::IncorrectSourceType;

using sf::core::model::Event;
using sf::core::model::EventRef;

using sf::core::model::EventDrain;
using sf::core::model::EventDrainManager;
using sf::core::model::EventDrainRef;

using sf::core::model::EventSource;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManager;


class FailEvent : public Event {
 public:
  FailEvent() : Event("abc", "NULL") {}
  void handle() {
    throw IncorrectSourceType("");
  }
};


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
  void sendAck() {}

  int getFD() {
    return 0;
  }
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


class TestSource2 : public EventSource {
 public:
  TestSource2(std::string id) : EventSource(id) {}
  int getFD() {
    return 1;
  }
  EventRef parse() {
    return EventRef(nullptr);
  }
};


class TestLoopManager : public LoopManager {
 public:
  void add(EventSourceRef source) {
    this->sources[source->id()] = source;
  }
  
  void removeDrain(std::string id) {
    this->drains.erase(id);
  }

  void removeSource(std::string id) {
    this->sources.erase(id);
  }

  EventRef wait(int timeout) {
    return EventRef();
  }
};


TEST(Event, Rescue) {
  FailEvent event;
  try {
    event.handle();
  } catch (SfException&) {
    ASSERT_THROW(event.rescue(std::current_exception()), IncorrectSourceType);
  }
}


TEST(EventDrain, Id) {
  TestDrain drain("id");
  ASSERT_EQ("id", drain.id());
}

TEST(EventDrainManager, Add) {
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

TEST(LoopManager, Get) {
  TestLoopManager manager;
  manager.add(EventSourceRef(new TestSource("id")));
  TestSource* source = manager.get<TestSource>("id");
  ASSERT_NE(nullptr, source);
}

TEST(LoopManager, NotFound) {
  TestLoopManager manager;
  ASSERT_THROW(manager.get<TestSource>("id"), EventSourceNotFound);
}

TEST(LoopManager, WrongType) {
  TestLoopManager manager;
  manager.add(EventSourceRef(new TestSource("id")));
  ASSERT_THROW(manager.get<TestSource2>("id"), IncorrectSourceType);
}
