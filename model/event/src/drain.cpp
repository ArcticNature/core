// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <string>

#include "core/interface/lifecycle.h"
#include "core/model/event.h"


using sf::core::interface::Lifecycle;

using sf::core::lifecycle::EVENT_DRAIN_ENQUEUE;
using sf::core::lifecycle::DrainEnqueueArg;

using sf::core::model::EventDrain;


DrainEnqueueArg::DrainEnqueueArg(std::string drain) {
  this->_added = false;
  this->_drain = drain;
}

void DrainEnqueueArg::add() {
  this->_added = true;
}

bool DrainEnqueueArg::added() const {
  return this->_added;
}

std::string DrainEnqueueArg::drain() const {
  return this->_drain;
}


EventDrain::EventDrain(std::string drain) : drain_id(drain) {
  // NOOP.
}

EventDrain::~EventDrain() {
  // NOOP.
}

void EventDrain::enqueue(const std::string& chunk) {
  this->buffer.push_back(chunk);
  DrainEnqueueArg arg(this->id());
  Lifecycle::trigger(EVENT_DRAIN_ENQUEUE, &arg);
}

std::string EventDrain::id() const {
  return this->drain_id;
}

void EventDrain::rescue(std::exception_ptr ex) {
  std::rethrow_exception(ex);
}
