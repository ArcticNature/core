// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <memory>

#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/exceptions/event.h"
#include "core/model/event.h"

#include "./fixtures.h"


using sf::core::exception::DuplicateItem;
using sf::core::exception::EventSourceNotFound;
using sf::core::exception::IncorrectSourceType;
using sf::core::exception::ItemNotFound;

using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;

using sf::core::model::LoopManager;
using sf::core::model::RefStore;

using sf::testing::TestDrain;
using sf::testing::TestSource;


class TestContainer {
 protected:
  int _fd;
  std::string _id;

 public:
  TestContainer(std::string id, int fd) {
    this->_id = id;
    this->_fd = fd;
  }

  int fd() {
    return this->_fd;
  }

  std::string id() const {
    return this->_id;
  }
};
typedef std::shared_ptr<TestContainer> TestRef;


class TestStore : public RefStore<TestRef> {
 public:
  int length() {
    return this->by_id.size();
  }
};


TEST(RefStore, Add) {
  TestRef ref(new TestContainer("a", 1));
  TestStore store;
  store.add(ref);
  ASSERT_EQ(1, store.length());
}

TEST(RefStore, AddTwice) {
  TestRef ref(new TestContainer("a", 1));
  TestStore store;
  store.add(ref);
  ASSERT_THROW(store.add(ref), DuplicateItem);
}

TEST(RefStore, GetByFd) {
  TestRef ref(new TestContainer("a", 1));
  TestStore store;
  store.add(ref);
  ASSERT_EQ(ref.get(), store.get(1).get());
}

TEST(RefStore, GetByFdMissing) {
  TestStore store;
  ASSERT_THROW(store.get(1), ItemNotFound);
}

TEST(RefStore, GetById) {
  TestRef ref(new TestContainer("a", 1));
  TestStore store;
  store.add(ref);
  ASSERT_EQ(ref.get(), store.get("a").get());
}

TEST(RefStore, GetByIdMissing) {
  TestStore store;
  ASSERT_THROW(store.get("a"), ItemNotFound);
}

TEST(RefStore, RemoveByFd) {
  TestRef ref(new TestContainer("a", 1));
  TestStore store;
  store.add(ref);
  store.remove(1);
  ASSERT_THROW(store.get("a"), ItemNotFound);
}

TEST(RefStore, RemoveByFdMissing) {
  TestStore store;
  ASSERT_THROW(store.remove(1), ItemNotFound);
}

TEST(RefStore, RemoveById) {
  TestRef ref(new TestContainer("a", 1));
  TestStore store;
  store.add(ref);
  store.remove("a");
  ASSERT_THROW(store.get(1), ItemNotFound);
}

TEST(RefStore, RemoveByIdMissing) {
  TestStore store;
  ASSERT_THROW(store.remove("a"), ItemNotFound);
}


class TestLoop : public LoopManager {
 public:
  int fdFor(EventDrainRef drain) const {
    return LoopManager::fdFor(drain);
  }

  int fdFor(EventSourceRef source) const {
    return LoopManager::fdFor(source);
  }

  void add(EventDrainRef drain) {
    this->drains.add(drain);
  }

  void add(EventSourceRef source) {
    this->sources.add(source);
  }

  void removeDrain(std::string id) {
    this->drains.remove(id);
  }

  void removeSource(std::string id) {
    this->sources.remove(id);
  }

  EventRef wait(int timeout = -1) {
    return EventRef();
  }
};


TEST(LoopManager, FetchSource) {
  EventSourceRef source(new TestSource());
  TestLoop loop;
  loop.add(source);
  ASSERT_EQ(source.get(), loop.fetch(source->id()).get());
}

TEST(LoopManager, FetchSourceFail) {
  TestLoop loop;
  ASSERT_THROW(loop.fetch("source"), EventSourceNotFound);
}

TEST(LoopManager, Downcast) {
  EventSourceRef source(new TestSource());
  TestLoop loop;

  loop.add(source);
  TestSource* test = loop.downcast<TestSource>("test-source");
  ASSERT_EQ(source.get(), test);
}

TEST(LoopManager, DowncastMissing) {
  TestLoop loop;
  ASSERT_THROW(
      loop.downcast<TestSource>("test-source"),
      EventSourceNotFound
  );
}

TEST(LoopManager, DowncastFail) {
  EventSourceRef source(new TestSource());
  TestLoop loop;
  loop.add(source);
  ASSERT_THROW(
      loop.downcast<TestDrain>("test-source"),
      IncorrectSourceType
  );
}

TEST(LoopManager, GetDrainFd) {
  TestLoop loop;
  EventDrainRef drain(new TestDrain());
  int fd = loop.fdFor(drain);
  ASSERT_EQ(-1, fd);
}

TEST(LoopManager, GetSourceFd) {
  TestLoop loop;
  EventSourceRef source(new TestSource());
  int fd = loop.fdFor(source);
  ASSERT_EQ(-1, fd);
}
