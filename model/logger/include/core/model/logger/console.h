// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_LOGGER_CONSOLE_H_
#define CORE_MODEL_LOGGER_CONSOLE_H_

#include <string>

#include "core/model/logger.h"


namespace sf {
namespace core {
namespace model {

  //! Log to the console.
  /*
   * Core logger that is always available and can be
   * used as the emmergency logger.
   */
  class ConsoleLogger : public Logger {
   public:
    explicit ConsoleLogger(std::string);
    ~ConsoleLogger();
    virtual void log(
        const LogLevel level, const std::string message,
        LogInfo variables
    );
  };

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_LOGGER_CONSOLE_H_
