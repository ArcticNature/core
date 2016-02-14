// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/event.h"

#include <fcntl.h>

#include <map>
#include <string>
#include <utility>

#include "core/context/static.h"
#include "core/exceptions/event.h"


using sf::core::context::Static;
using sf::core::exception::SfException;
using sf::core::exception::EventDrainNotFound;

using sf::core::model::Event;
using sf::core::model::EventDrain;
using sf::core::model::EventDrainManager;
using sf::core::model::EventDrainRef;
using sf::core::model::EventSource;
using sf::core::model::EventSourceManager;


Event::Event(std::string correlation, std::string drain) {
  this->correlation_id = correlation;
  this->drain_id = drain;
}
Event::~Event() {}


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


class NullDrain : public EventDrain {
 protected:
  int null_fd;

 public:
  NullDrain() : EventDrain("NULL") {
    this->null_fd = -1;
  }

  ~NullDrain() {
    if (this->null_fd != -1) {
      Static::posix()->close(this->null_fd);
    }
  }

  int getFD() {
    if (this->null_fd == -1) {
      this->null_fd = Static::posix()->open("/dev/null", O_WRONLY, 0);
    }
    return this->null_fd;
  }

  void sendAck() {}
};


EventDrainManager::EventDrainManager() {
  this->add(EventDrainRef(new NullDrain()));
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
  this->drains.erase(id);
}


EventSource::EventSource(std::string id) : source_id(id) {}
EventSource::~EventSource() {}

std::string EventSource::id() const {
  return this->source_id;
}


EventSourceManager::~EventSourceManager() {}
