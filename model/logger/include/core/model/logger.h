// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_LOGGER_H_
#define CORE_MODEL_LOGGER_H_

#include <map>
#include <memory>
#include <string>


namespace sf {
namespace core {
namespace model {

  //! Short-end for message variables.
  typedef std::map<std::string, std::string> LogInfo;

  //! Log event level.
  enum LogLevel {
    LL_ERROR   = 0,
    LL_WARNING = 1,
    LL_INFO    = 2,
    LL_DEBUG   = 3
  };

  class Logger;
  typedef std::shared_ptr<Logger> LoggerRef;

  //! Abstract logging interface.
  /*!
   * GCC has some interesting macros that could be used in formats.
   * https://gcc.gnu.org/onlinedocs/gcc-4.2.1/cpp/Common-Predefined-Macros.html
   */
  class Logger {
   protected:
    //! Signleton Logger instance for fallback.
    static LoggerRef fallback_instance;

   public:
    //! Returns the fallback Logger instance.
    /*
     * Logger instances are created after the configuration has been loaded
     * and are configured to the user requirements.
     *
     * For cases where such instances cannot be used (for example during
     * initialisation or in the Spawner), the fallback instance can be used.
     */
    static LoggerRef fallback();

    //! Cleans up the fallback logger.
    static void destroyFallback();

   protected:
    //! Format of log messages.
    /*!
     * The format is a string that include variable place-holders defined
     * with the syntax ${variable}, where variable is the name of the variable
     * to lookup in the LogInfo argument.
     */
    std::string format;

    //! Format a log messages.
    /*!
     * Helper method to format a log message based on the value of format and
     * the variables passed to the logging method.
     */
    std::string formatMessage(
        const LogLevel level, const std::string message,
        LogInfo vars
    );

   public:
    explicit Logger(std::string format);
    virtual ~Logger();

    virtual void log(
        const LogLevel level, const std::string message,
        LogInfo variables
    ) = 0;
  };


// Helper macros.
#define STR(str)  #str
#define STR_(str) STR(str)

//! Return the current line as a string.
#define __STR_LINE__ STR_(__LINE__)

//! Extend the given LogInfo instance with defaults and return it.
#define LOG_ADD_VARS(variables) (variables["line"] = __STR_LINE__, \
  variables["file"]     = __FILE__,                                \
  variables["function"] = __PRETTY_FUNCTION__,                     \
  variables                                                        \
)

#define LOG(logger, level, message) {           \
  sf::core::model::LogInfo _log_varaibles;       \
  LOG_ADD_VARS(_log_varaibles);                 \
  logger->log(level, message, _log_varaibles);  \
}

#define LLERROR   sf::core::model::LogLevel::LL_ERROR
#define LLWARNING sf::core::model::LogLevel::LL_WARNING
#define LLINFO    sf::core::model::LogLevel::LL_INFO
#define LLDEBUG   sf::core::model::LogLevel::LL_DEBUG

#define ERROR(logger, message)   LOG(logger, LLERROR,   message)
#define WARNING(logger, message) LOG(logger, LLWARNING, message)
#define INFO(logger, message)    LOG(logger, LLINFO,    message)
#define DEBUG(logger, message)   LOG(logger, LLDEBUG,   message)

#define LOGV(logger, level, message, variables) \
  logger->log(level, message, LOG_ADD_VARS(variables))

#define ERRORV(log, msg, vars)   LOGV(log, LLERROR,   msg, vars)
#define WARNINGV(log, msg, vars) LOGV(log, LLWARNING, msg, vars)
#define INFOV(log, msg, vars)    LOGV(log, LLINFO,    msg, vars)
#define DEBUGV(log, msg, vars)   LOGV(log, LLDEBUG,   msg, vars)

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_LOGGER_H_
