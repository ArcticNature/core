// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/context/context.h"
#include "core/model/logger.h"


using sf::core::context::Context;
using sf::core::context::ProxyLogger;

using sf::core::model::LogInfo;
using sf::core::model::LogLevel;
using sf::core::model::Logger;


ProxyLogger::ProxyLogger(std::string component) : Logger("") {
  this->component_ = component;
}

ProxyLogger* ProxyLogger::operator->() {
  return this;
}

void ProxyLogger::log(
    LogLevel level, const std::string message,
    LogInfo variables
) {
  variables["<component>"] = this->component_;
  Context::Logger()->log(level, message, variables);
}
