// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <gtest/gtest.h>

#include "core/model/event.h"

#include "./fixtures.h"


using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::ReThrowEvent;

using sf::testing::FailEvent;
using sf::testing::NoopEvent;
using sf::testing::TestDrain;


TEST(Event, StoresADrain) {
  EventDrainRef drain(new TestDrain());
  EventRef event(new NoopEvent("", drain));
  ASSERT_EQ(drain.get(), event->drain().get());
}

TEST(Event, CorrelationID) {
  NoopEvent event("correlate");
  ASSERT_EQ("correlate", event.correlation());
}

TEST(Event, SetsIDAndCorrelation) {
  NoopEvent event("");
  ASSERT_EQ("new-id", event.id("new-id"));
  ASSERT_EQ("new-id", event.correlation());
  ASSERT_EQ("new-id", event.id());
}

TEST(Event, SetsIDOnly) {
  NoopEvent event("correlate");
  ASSERT_EQ("new-id", event.id("new-id"));
  ASSERT_EQ("new-id", event.id());
  ASSERT_EQ("correlate", event.correlation());
}

TEST(Event, SetsIDOnce) {
  NoopEvent event("");
  ASSERT_EQ("old-id", event.id("old-id"));
  ASSERT_EQ("old-id", event.id("new-id"));
  ASSERT_EQ("old-id", event.id());
}

TEST(Event, ReThrowsByDefault) {
  std::exception_ptr ex;
  FailEvent event("");

  try {
    event.handle();
  } catch (std::exception&) {
    ex = std::current_exception();
  }

  ASSERT_THROW(event.rescue(ex), std::exception);
}


TEST(ReThrowEvent, HandleThrows) {
  std::exception_ptr ex = std::make_exception_ptr(std::runtime_error("test"));
  EventDrainRef drain(new TestDrain());
  ReThrowEvent  event(ex, "", drain);
  ASSERT_THROW(event.handle(), std::runtime_error);
}
