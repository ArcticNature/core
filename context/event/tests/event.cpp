// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/event.h"
#include "core/exceptions/base.h"
#include "core/lifecycle/process.h"

using sf::core::context::EventContext;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::DuplicateItem;
using sf::core::lifecycle::Process;

typedef EventContext<int> TestEventContext;


class EventContextTest : public ::testing::Test {
 protected:
  ~EventContextTest() {
    TestEventContext::destroy();
  }
};


TEST_F(EventContextTest, StoreAndPop) {
  TestEventContext::store("abc", 2);
  ASSERT_EQ(2, TestEventContext::pop("abc"));
}

TEST_F(EventContextTest, PopMissing) {
  ASSERT_THROW(TestEventContext::pop("abc"), ContextUninitialised);
}

TEST_F(EventContextTest, Destroy) {
  TestEventContext::store("abc", 2);
  Process::Exit();
  ASSERT_THROW(TestEventContext::pop("abc"), ContextUninitialised);
}

TEST_F(EventContextTest, PopTwice) {
  TestEventContext::store("abc", 2);
  ASSERT_EQ(2, TestEventContext::pop("abc"));
  ASSERT_THROW(TestEventContext::pop("abc"), ContextUninitialised);
}

TEST_F(EventContextTest, StoreTwice) {
  TestEventContext::store("abc", 2);
  ASSERT_THROW(TestEventContext::store("abc", 1), DuplicateItem);
}
