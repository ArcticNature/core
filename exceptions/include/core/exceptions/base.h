// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_EXCEPTIONS_BASE_H_
#define CORE_EXCEPTIONS_BASE_H_

/*
 * All exception classes have a unique code to identify them.
 * Codes are also grouped by exceptions group to help avoid clashes.
 * The groups are:
 *   * 0 < code:              System errors, mostly from errno.
 *   * -1000 < code <= 0:     Common or unclassified exceptions.
 *   * -2000 < code <= -1000: LUA exceptions.
 *   * -3000 < code <= -2000: Configuration exceptions.
 *   * -4000 < code <= -3000: Spawner exceptions.
 *   * -5000 < code <= -4000: GitException from GitRepo.
 */

#include <exception>
#include <string>
#include <vector>


#define MSG_EXCEPTION(base, name)   \
class name : public base {          \
 public:                            \
  explicit name(std::string);       \
  virtual int getCode() const;      \
};

#define NO_ARG_EXCEPTION(base, name)  \
class name : public base {            \
 public:                              \
  name();                             \
  virtual int getCode() const;        \
};

#define CHECK_POSITIVE_ERRNO(function, message, ...)   \
if (function(__VA_ARGS__) < 0) {                       \
  throw sf::core::exception::ErrNoException(message);  \
}

#define CHECK_ZERO_ERRNO(function, message, ...)       \
if (function(__VA_ARGS__) != 0) {                      \
  throw sf::core::exception::ErrNoException(message);  \
}

#define MSG_DEFINITION(base, name, code)     \
name::name(std::string msg) : base(msg) { }  \
int name::getCode() const { return code; }

#define MSG_DEFINITION_PREFIX(base, name, code, prefix)  \
name::name(std::string msg) : base(prefix + msg) { }     \
int name::getCode() const { return code; }

#define NO_ARG_DEFINITION(base, name, code, message)   \
int name::getCode() const { return code; }             \
name::name() : base(message) { }


namespace sf {
namespace core {
namespace exception {

  //! Base class for internal exceptions.
  /*!
   * Each exception must have both a user friendly message and an error code.
   * 
   * The code is unique to each exception and not to the specific occurrence
   * of an error.
   * If different contexts throwing the same exception would benefit from
   * different codes than different exceptions should be defined.
   */
  class SfException : virtual public std::exception {
   protected:
    std::string message;  //!< Store the user friendly error message.

    //! Backtrace of when the exception was thrown.
    std::vector<std::string> trace;

   public:
    //! Creates a new exception.
    /*!
     * \param message The user friendly message describing the event.
     */
    explicit SfException(std::string message);
    ~SfException() throw();

    //! Returns the error code for this exception.
    virtual int getCode() const = 0;
    virtual std::string getTrace() const;
    virtual std::vector<std::string> stackTrace() const;

    virtual const char* what() const throw();
  };

  //! Abort the execution of the process due to an exception.
  /*!
   * Instances of this exception take the infirmation of the
   * original exception.
   */
  class AbortException : public SfException {
   protected:
    int code;

   public:
    explicit AbortException(const SfException& origin);
    AbortException(
        std::string message, int code,
        std::vector<std::string> trace
    );

    virtual int getCode() const;
  };

  //! Errno based wrapper for C errors.
  class ErrNoException : public SfException {
   protected:
    int error_number;  //!< Store the errno at the time of creation.

   public:
    explicit ErrNoException(std::string message);
    virtual int getCode() const;
  };

  class SocketException : virtual public SfException {
   protected:
    int code;

   public:
    SocketException(int code, std::string message);
    virtual int getCode() const;
  };

  //! Thrown when a read is attempted on a channel with no data.
  NO_ARG_EXCEPTION(SfException, ChannelEmpty);

  //! Thrown when the process wants to terminate cleanly.
  NO_ARG_EXCEPTION(SfException, CleanExit);

  //! Thrown when an unitialised context variable is accessed.
  MSG_EXCEPTION(SfException, ContextUninitialised);

  //! Thrown when corrupted data is read by the system.
  MSG_EXCEPTION(SfException, CorruptedData);

  //! Thrown by the injection system when an injection has occurred already.
  MSG_EXCEPTION(SfException, DuplicateInjection);

  //! Thrown when a requested factory was not registered.
  MSG_EXCEPTION(SfException, FactoryNotFound);

  //! Thrown when an invalid file is being used.
  MSG_EXCEPTION(SfException, FileError);

  //! Thrown when a fork operation fails.
  NO_ARG_EXCEPTION(SfException, ForkFailed);

  //! Thrown when a requested group was not found.
  MSG_EXCEPTION(SfException, GroupNotFound);

  //! Thrown when an impossible location in the code is reached.
  NO_ARG_EXCEPTION(SfException, ImpossiblePath);

  //! Thrown by a command line parser when an invalid option is provided.
  MSG_EXCEPTION(SfException, InvalidCommandLine);

  //! Thrown during daemonisation if a new session could not be created.
  NO_ARG_EXCEPTION(SfException, InvalidDaemonSession);

  //! Thrown when an operation on a dynamic object is not allowed.
  MSG_EXCEPTION(SfException, InvalidValueOperation);

  //! Thrown when a method that has not been implemented is called.
  MSG_EXCEPTION(SfException, NotImplemented);

  //! Thrown when an operation that is not permitted is attempted.
  NO_ARG_EXCEPTION(SfException, OperationNotPermitted);

  MSG_EXCEPTION(SfException, ProcessNotFound);

  //! Thrown when a requested service was not found.
  MSG_EXCEPTION(SfException, ServiceNotFound);

  //! Thrown when the handling of an event needs to be interrupted.
  NO_ARG_EXCEPTION(SfException, StopException);

  //! Thrown when an invalid type is passed to an argument.
  MSG_EXCEPTION(SfException, TypeError);

  //! Thrown when a source receives a request for an unknown event.
  MSG_EXCEPTION(SfException, UnrecognisedEvent);

  //! Thrown when an unsupported mode is requested.
  NO_ARG_EXCEPTION(SfException, UnsupportedMode);

  //! Thrown when a requested user was not found.
  MSG_EXCEPTION(SfException, UserNotFound);

}  // namespace exception
}  // namespace core
}  // namespace sf

#endif  // CORE_EXCEPTIONS_BASE_H_
