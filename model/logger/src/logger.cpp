// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/logger.h"

#include <memory>
#include <sstream>
#include <string>

#include "core/compile-time/options.h"
#include "core/model/logger/console.h"
#include "core/utility/string.h"

#define FALLBACK_PREFIX "[Fallback] "

using sf::core::model::ConsoleLogger;
using sf::core::model::Logger;
using sf::core::model::LogLevel;
using sf::core::model::LogInfo;

using sf::core::utility::string::currentTimestamp;
using sf::core::utility::string::findFirsNotEscaped;


std::string LEVEL_NAMES[] = {
  "ERROR", "WARNING", "INFO", "DEBUG"
};


// Static code.
std::shared_ptr<Logger> sf::core::model::Logger::fallback_instance;

Logger* Logger::fallback() {
  if (Logger::fallback_instance.get() == nullptr) {
    Logger::fallback_instance = std::shared_ptr<Logger>(
        new ConsoleLogger(FALLBACK_PREFIX DEFAULT_LOG_FORMAT)
    );
  }
  return Logger::fallback_instance.get();
}


// Instance code.
Logger::Logger(std::string format) {
  this->format = format;
}
Logger::~Logger() {}


std::string Logger::formatMessage(
    const LogLevel level, const std::string message, LogInfo vars
) {
  std::stringstream buffer;
  std::string::size_type pos = findFirsNotEscaped("${message}", this->format);

  if (pos == -1) {
    buffer << this->format;
  } else {
    buffer << this->format.substr(0, pos) << message;
    buffer << this->format.substr(pos + 10);
  }

  std::string format = buffer.str();
  pos = findFirsNotEscaped("${", format);

  if (pos == -1) {
    return format;
  }

  // Extend variables with default stuff.
  vars["level"] = LEVEL_NAMES[level];
  if (vars.find("<now>") == vars.end()) {
    vars["<now>"] = currentTimestamp();
  }

  buffer.clear();
  buffer.str("");
  std::string::size_type close = 0;
  std::string::size_type prev = 0;

  while (pos != -1) {
    buffer << format.substr(prev, pos - prev);
    close = findFirsNotEscaped("}", format, pos);

    if (close == -1) {
      break;
    }

    std::string key = format.substr(pos + 2, close - pos - 2);
    buffer << vars[key];
    prev = close + 1;
    pos = findFirsNotEscaped("${", format, close);
  }

  buffer << format.substr(prev);

#ifdef DEBUG_BUILD
  if (vars.find("trace") != vars.end()) {
    buffer << " Trace: " << vars["trace"];
  }
#endif  // DEBUG_BUILD

  return buffer.str();
}
