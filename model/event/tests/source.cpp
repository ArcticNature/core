// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/model/event.h"

#include "./fixtures.h"


using sf::core::exception::CleanExit;
using sf::core::exception::SfException;
using sf::core::model::EventRef;

using sf::testing::TestSource;


class FailParse : public TestSource {
 protected:
  EventRef parse() {
    throw CleanExit();
  }

 public:
  bool saved;

  FailParse() : TestSource() {
    this->saved = false;
  }

  void rescue(std::exception_ptr ex) {
    this->saved = true;
  }
};


TEST(EventSource, Id) {
  TestSource source;
  ASSERT_EQ("test-source", source.id());
}

TEST(EventSource, FetchNullEvent) {
  TestSource source;
  EventRef event = source.fetch();
  ASSERT_EQ(nullptr, event.get());
}

TEST(EventSource, FetchRescuesErrors) {
  FailParse source;
  source.fetch();
  ASSERT_TRUE(source.saved);
}

TEST(EventSource, ReThrows) {
  TestSource source;
  std::exception_ptr ex;

  try {
    throw std::exception();
  } catch (std::exception&) {
    ex = std::current_exception();
  }

  ASSERT_THROW(source.rescue(ex), std::exception);
}
