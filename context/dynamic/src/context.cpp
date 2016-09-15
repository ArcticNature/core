// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/context.h"

#include "core/exceptions/base.h"
#include "core/model/logger.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::exception::ContextUninitialised;

using sf::core::model::Logger;
using sf::core::model::LoggerRef;
using sf::core::model::EventSourceManagerRef;


ContextRef Context::_instance;


LoggerRef Context::logger() {
  return Context::instance()->_logger;
}

EventSourceManagerRef Context::sourceManager() {
  EventSourceManagerRef manager = Context::instance()->source_manager;
  if (manager.get() == nullptr) {
    throw ContextUninitialised("Event source manager not initialised.");
  }
  return manager;
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

Context::~Context() {
  this->source_manager = EventSourceManagerRef();
}

void Context::initialise(LoggerRef logger) {
  this->_logger = logger;
}

void Context::initialise(EventSourceManagerRef manager) {
  this->source_manager = manager;
}

EventSourceManagerRef Context::instanceSourceManager() {
  return this->source_manager;
}


void Context::destroy() {
  Context::_instance = ContextRef();
}


// Some methods should not be used in release versions but
// are needed for tests and could be helpful for debugging.
#if DEBUG_BUILD

void Context::reset() {
  Context::_instance = ContextRef();
}

#endif  // DEBUG_BUILD
