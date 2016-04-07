// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/event.h"

#include <map>
#include <string>
#include <utility>

#include "core/exceptions/event.h"


using sf::core::exception::SfException;
using sf::core::exception::EventDrainNotFound;

using sf::core::model::Event;
using sf::core::model::EventDrain;
using sf::core::model::EventDrainManager;
using sf::core::model::EventDrainRef;
using sf::core::model::EventSource;
using sf::core::model::EventSourceManager;


Event::Event(std::string correlation, std::string drain) {
  this->_id = "";
  this->correlation_id = correlation;
  this->drain_id = drain;
}
Event::~Event() {}

std::string Event::correlation() const {
  return this->correlation_id;
}

std::string Event::id(std::string id) {
  if (this->_id == "") {
    this->_id = id;
  }
  if (this->correlation_id == "") {
    this->correlation_id = this->_id;
  }
  return this->_id;
}

std::string Event::id() const {
  return this->_id;
}


EventDrain::EventDrain(std::string drain) : drain_id(drain) {
  this->outstanding = 0;
}
EventDrain::~EventDrain() {}

std::string EventDrain::id() const {
  return this->drain_id;
}

bool EventDrain::handleError(SfException* ex) {
  return false;
}


void EventDrainManager::add(EventDrainRef drain) {
  this->drains.insert(
      std::pair<std::string, EventDrainRef>(drain->id(), drain)
  );
}

EventDrainRef EventDrainManager::get(std::string id) const {
  std::map<std::string, EventDrainRef>::const_iterator drain;
  drain = this->drains.find(id);

  if (drain == this->drains.end()) {
    throw EventDrainNotFound(id);
  }
  return drain->second;
}

void EventDrainManager::remove(std::string id) {
  if (this->drains.find(id) == this->drains.end()) {
    throw EventDrainNotFound(id);
  }
  this->drains.erase(id);
}


EventSource::EventSource(std::string id) : source_id(id) {}
EventSource::~EventSource() {}

std::string EventSource::id() const {
  return this->source_id;
}


EventSourceManager::~EventSourceManager() {}
