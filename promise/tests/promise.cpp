// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <poolqueue/Promise.hpp>

#include "core/exceptions/base.h"
#include "core/promise.h"


using poolqueue::Promise;

using sf::core::PromiseKeeper;
using sf::core::exception::DuplicateItem;
using sf::core::exception::ItemNotFound;


class TestKeeper : public PromiseKeeper {
 public:
  int size() {
    return this->promises_.size();
  }

  unsigned int ttlFor(std::string id) {
    return this->promises_[id].ttl;
  }
};


class PromiseKeeperTest : public ::testing::Test {
 protected:
  TestKeeper keeper;
};

TEST_F(PromiseKeeperTest, Clear) {
  Promise promise;
  this->keeper.keep("p1", promise);
  this->keeper.keep("p2", promise);
  ASSERT_EQ(2, this->keeper.size());

  this->keeper.clear();
  ASSERT_EQ(0, this->keeper.size());
}

TEST_F(PromiseKeeperTest, Keep) {
  Promise promise;
  this->keeper.keep("promise", promise);
  ASSERT_EQ(1, this->keeper.size());
}

TEST_F(PromiseKeeperTest, KeepOne) {
  Promise promise;
  this->keeper.keep("promise", promise);
  ASSERT_THROW(this->keeper.keep("promise", promise), DuplicateItem);
  ASSERT_EQ(1, this->keeper.size());
}

TEST_F(PromiseKeeperTest, Pop) {
  Promise p1;
  Promise p2;
  this->keeper.keep("promise", p1);
  p2 = this->keeper.pop("promise");
  ASSERT_EQ(p1, p2);
  ASSERT_EQ(0, this->keeper.size());
}

TEST_F(PromiseKeeperTest, PopMissing) {
  ASSERT_THROW(this->keeper.pop("promise"), ItemNotFound);
}

TEST_F(PromiseKeeperTest, TtlSet) {
  Promise promise;
  this->keeper.keep("promise", promise, 2);
  unsigned int ttl = this->keeper.ttlFor("promise");
  ASSERT_EQ(2, ttl);
}

TEST_F(PromiseKeeperTest, TtlDecrease) {
  Promise promise;
  this->keeper.keep("promise", promise, 2);
  this->keeper.tick();
  unsigned int ttl = this->keeper.ttlFor("promise");
  ASSERT_EQ(1, ttl);
}

TEST_F(PromiseKeeperTest, TtlExpires) {
  bool rejected = false;
  Promise promise;
  promise.except([&rejected](const std::exception_ptr& e) {
      rejected = true;
      return nullptr;
  });

  this->keeper.keep("promise", promise, 2);
  this->keeper.tick();
  this->keeper.tick();
  ASSERT_EQ(0, this->keeper.size());
  ASSERT_TRUE(rejected);
}

TEST_F(PromiseKeeperTest, TtlNoExpires) {
  bool rejected = false;
  Promise promise;
  promise.except([&rejected](const std::exception_ptr& e) {
      rejected = true;
      return nullptr;
  });

  this->keeper.keep("promise", promise);
  this->keeper.tick();
  this->keeper.tick();
  ASSERT_EQ(1, this->keeper.size());
  ASSERT_FALSE(rejected);
}
