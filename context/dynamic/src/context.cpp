// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/context.h"

#include "core/model/logger.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;

using sf::core::model::Logger;
using sf::core::model::LoggerRef;
using sf::core::model::EventSourceManagerRef;


ContextRef Context::_instance;


LoggerRef Context::logger() {
  return Context::instance()->_logger;
}

EventSourceManagerRef Context::sourceManager() {
  return Context::instance()->source_manager;
}

void Context::initialise(ContextRef context) {
  Context::_instance = context;
}

ContextRef Context::instance() {
  if (Context::_instance.get() == nullptr) {
    Context::_instance = ContextRef(new Context());
  }
  return Context::_instance;
}


Context::Context() {
  this->_logger = Logger::fallback();
}

void Context::initialise(LoggerRef logger) {
  this->_logger = logger;
}

void Context::initialise(EventSourceManagerRef manager) {
  this->source_manager = manager;
}


// Some methods should not be used in release versions but
// are needed for tests and could be helpful for debugging.
#if DEBUG_BUILD

void Context::reset() {
  Context::_instance = ContextRef();
}

#endif  // DEBUG_BUILD
