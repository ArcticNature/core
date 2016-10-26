// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <string>

#include "core/model/event.h"


using sf::core::model::EventDrain;


EventDrain::EventDrain(std::string drain) : drain_id(drain) {
  // NOOP.
}

EventDrain::~EventDrain() {
  // NOOP.
}

void EventDrain::enqueue(const std::string& chunk) {
  this->buffer.push_back(chunk);
}

std::string EventDrain::id() const {
  return this->drain_id;
}

void EventDrain::rescue(std::exception_ptr ex) {
  std::rethrow_exception(ex);
}
