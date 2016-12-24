// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/exceptions/base.h"
#include "core/exceptions/event.h"
#include "core/model/event.h"


using sf::core::exception::ItemNotFound;
using sf::core::exception::EventSourceNotFound;

using sf::core::model::EventDrainRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManager;


int LoopManager::fdFor(EventDrainRef drain) const {
  return drain->fd();
}

int LoopManager::fdFor(EventSourceRef source) const {
  return source->fd();
}

void LoopManager::processDrain(int fd) {
  EventDrainRef drain = this->drains.get(fd);
  bool empty = drain->flush();
  if (empty) {
    this->removeDrain(drain->id());
  }
}


LoopManager::~LoopManager() {
  // Noop.
}

EventSourceRef LoopManager::fetch(std::string id) const {
  try {
    return this->sources.get(id);
  } catch(ItemNotFound&) {
    throw EventSourceNotFound(id);
  }
}
