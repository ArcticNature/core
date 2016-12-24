// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <string>

#include "core/exceptions/base.h"
#include "core/interface/lifecycle.h"
#include "core/model/event.h"


using sf::core::exception::CorruptedData;
using sf::core::interface::Lifecycle;

using sf::core::lifecycle::EVENT_DRAIN_ENQUEUE;
using sf::core::lifecycle::DrainEnqueueArg;

using sf::core::model::EventDrain;
using sf::core::model::EventDrainBuffer;
using sf::core::model::EventDrainBufferRef;


EventDrainBuffer::EventDrainBuffer(uint32_t size) {
  this->buffer = new char[size];
  this->size = size;
  this->consumed = 0;
}

EventDrainBuffer::~EventDrainBuffer() {
  if (this->buffer) {
    delete [] this->buffer;
  }
}

void EventDrainBuffer::consume(uint32_t amount) {
  this->consumed += amount;
  if (this->consumed > this->size) {
    this->consumed = this->size;
  }
}

void* EventDrainBuffer::data(uint32_t offset) {
  if (offset >= this->size) {
    throw CorruptedData("Unable to access data after the end of the buffer");
  }
  return this->buffer + offset;
}

bool EventDrainBuffer::empty() const {
  return this->consumed >= this->size;
}

char* EventDrainBuffer::remaining(uint32_t* left) const {
  char* start = this->buffer;
  uint32_t offset = this->consumed;
  if (offset >= this->size) {
    throw CorruptedData("Unable to access data after the end of the buffer");
  }
  if (left) {
    *left = this->size - offset;
  }
  return start + offset;
}


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

void EventDrain::enqueue(EventDrainBufferRef chunk) {
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
