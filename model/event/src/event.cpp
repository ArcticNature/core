// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/event.h"

#include <exception>
#include <string>


using sf::core::model::Event;
using sf::core::model::EventDrainRef;


Event::Event(std::string correlation, EventDrainRef drain) {
  this->event_id = "";
  this->correlation_id = correlation;
  this->_drain = drain;
}

Event::~Event() {
  // Noop.
}

std::string Event::correlation() const {
  return this->correlation_id;
}

EventDrainRef Event::drain() const {
  return this->_drain;
}

std::string Event::id(std::string id) {
  if (this->event_id == "") {
    this->event_id = id;
  }
  if (this->correlation_id == "") {
    this->correlation_id = this->event_id;
  }
  return this->event_id;
}

std::string Event::id() const {
  return this->event_id;
}

void Event::rescue(std::exception_ptr ex) {
  std::rethrow_exception(ex);
}
