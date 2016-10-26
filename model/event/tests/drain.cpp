// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <gtest/gtest.h>

#include "core/model/event.h"

#include "./fixtures.h"


using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;

using sf::testing::FailEvent;
using sf::testing::NoopEvent;
using sf::testing::TestDrain;


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
