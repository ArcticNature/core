// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/exceptions/base.h"
#include "core/exceptions/event.h"
#include "core/model/event.h"


using sf::core::exception::ItemNotFound;
using sf::core::exception::EventSourceNotFound;

using sf::core::model::EventSourceRef;
using sf::core::model::LoopManager;


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
