// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/logger/console.h"

#include <iostream>
#include <string>

using sf::core::model::ConsoleLogger;
using sf::core::model::Logger;
using sf::core::model::LogLevel;
using sf::core::model::LogInfo;


ConsoleLogger::ConsoleLogger(std::string format) : Logger(format) {
  INFO(this, "Initialised console logger.");
}

ConsoleLogger::~ConsoleLogger() {
  INFO(this, "Console logger cleand up.");
}


void ConsoleLogger::log(
    const LogLevel level, const std::string message,
    LogInfo variables
) {
  std::string formatted = this->formatMessage(level, message, variables);
  if (level <= LogLevel::LL_WARNING) {
    std::cerr << formatted << std::endl;
  } else {
    std::cout << formatted << std::endl;
  }
}
