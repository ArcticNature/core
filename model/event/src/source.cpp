// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <string>

#include "core/exceptions/base.h"
#include "core/model/event.h"


using sf::core::exception::SfException;
using sf::core::model::EventRef;
using sf::core::model::EventSource;


EventSource::EventSource(std::string id) : source_id(id) {
  // Noop.
}

EventSource::~EventSource() {
  // Noop.
}

std::string EventSource::id() const {
  return this->source_id;
}

EventRef EventSource::fetch() {
  try {
    return this->parse();
  } catch(SfException&) {
    this->rescue(std::current_exception());
  }
}

void EventSource::rescue(std::exception_ptr ex) {
  std::rethrow_exception(ex);
}
