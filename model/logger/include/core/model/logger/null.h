// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_LOGGER_NULL_H_
#define CORE_MODEL_LOGGER_NULL_H_

#include <string>

#include "core/model/logger.h"


namespace sf {
namespace core {
namespace model {

  //! Silent logger.
  /*
   * Core logger that is always available and can be used to
   * silence log messages.
   */
  class NullLogger : public Logger {
   public:
    NullLogger();
    virtual void log(
        const LogLevel level, const std::string message,
        LogInfo variables
    );
  };

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_LOGGER_NULL_H_
