// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/context.h"

#include "core/exceptions/base.h"
#include "core/model/logger.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::exception::ContextUninitialised;

using sf::core::model::Logger;
using sf::core::model::LoggerRef;
using sf::core::model::LoopManagerRef;


ContextRef Context::_instance;


void Context::Destroy() {
  Context::_instance = ContextRef();
}

void Context::Initialise(ContextRef context) {
  Context::_instance = context;
}

ContextRef Context::Instance() {
  if (Context::_instance.get() == nullptr) {
    Context::_instance = ContextRef(new Context());
  }
  return Context::_instance;
}

LoggerRef Context::Logger() {
  return Context::Instance()->_logger;
}

LoopManagerRef Context::LoopManager() {
  return Context::Instance()->loopManager();
}


Context::Context() {
  this->_logger = Logger::fallback();
}

Context::~Context() {
  this->loop_manager = LoopManagerRef();
}

void Context::initialise(LoggerRef logger) {
  this->_logger = logger;
}

void Context::initialise(LoopManagerRef manager) {
  this->loop_manager = manager;
}

LoopManagerRef Context::loopManager() {
  if (this->loop_manager.get() == nullptr) {
    throw ContextUninitialised("Loop manager not initialised.");
  }
  return this->loop_manager;
}
