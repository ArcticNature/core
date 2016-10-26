// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "./fixtures.h"

#include <string>

#include "core/model/event.h"


using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::EventDrain;
using sf::core::model::EventDrainRef;
using sf::core::model::EventSource;

using sf::testing::FailEvent;
using sf::testing::NoopEvent;
using sf::testing::TestDrain;
using sf::testing::TestSource;


FailEvent::FailEvent(std::string id) : Event(
    id, EventDrainRef(new TestDrain)
) {
  // Noop.
}

NoopEvent::NoopEvent(std::string id) : Event(
    id, EventDrainRef(new TestDrain)
) {
  // Noop.
}

void FailEvent::handle() {
  throw std::exception();
}


NoopEvent::NoopEvent(
    std::string id, EventDrainRef drain
) : Event(id, drain) {
  // Noop.
}

void NoopEvent::handle() {
  // Noop.
}


TestDrain::TestDrain() : EventDrain("test-drain") {
  // Noop.
}

bool TestDrain::flush() {
  return true;
}

int TestDrain::fd() {
  return -1;
}

int TestDrain::length() {
  return this->buffer.size();
}


TestSource::TestSource() : EventSource("test-source") {
  // Noop.
}

EventRef TestSource::parse() {
  return EventRef();
}

int TestSource::fd() {
  return -1;
}
