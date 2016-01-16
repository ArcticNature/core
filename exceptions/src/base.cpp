// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#include "core/exceptions/base.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <execinfo.h>
#include <string>
#include <sstream>
#include <vector>

using sf::core::exception::SfException;

using sf::core::exception::AbortException;
using sf::core::exception::ChannelEmpty;
using sf::core::exception::CleanExit;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::CorruptedData;
using sf::core::exception::DuplicateInjection;
using sf::core::exception::ErrNoException;
using sf::core::exception::FactoryNotFound;
using sf::core::exception::FileError;
using sf::core::exception::ForkFailed;
using sf::core::exception::GroupNotFound;
using sf::core::exception::ImpossiblePath;
using sf::core::exception::InvalidCommandLine;
using sf::core::exception::InvalidDaemonSession;
using sf::core::exception::InvalidValueOperation;
using sf::core::exception::NotImplemented;
using sf::core::exception::OperationNotPermitted;
using sf::core::exception::ServiceNotFound;
using sf::core::exception::SocketException;
using sf::core::exception::StopException;
using sf::core::exception::TypeError;
using sf::core::exception::UnrecognisedEvent;
using sf::core::exception::UnsupportedMode;
using sf::core::exception::UserNotFound;


AbortException::AbortException(const SfException& origin) : SfException(
    origin.what()
) {
  this->code  = origin.getCode();
  this->trace = origin.stackTrace();
}

AbortException::AbortException(
    std::string message, int code, std::vector<std::string> trace
) : SfException(message) {
  this->code  = code;
  this->trace = trace;
}

int AbortException::getCode() const {
  return this->code;
}


SfException::SfException(std::string message) {
  this->message = message;

  // Get stack trace.
  void*  buffer[50];
  size_t total   = backtrace(buffer, 50);
  char** symbols = backtrace_symbols(buffer, total);

  for (size_t idx = 0; idx < total; idx++) {
    // TODO(stefano): consider demangling names in trace for debug mode.
    // https://panthema.net/2008/0901-stacktrace-demangled/
    this->trace.push_back(std::string(symbols[idx]));
  }
  free(symbols);
}
SfException::~SfException() throw() { }

std::string SfException::getTrace() const {
  if (this->trace.size() == 0) {
    return "";
  }

  std::stringstream full_trace;
  std::vector<std::string>::const_iterator it = this->trace.begin();

  full_trace << *it;
  for (it++; it != this->trace.end(); it++) {
    full_trace << std::endl << *it;
  }
  return full_trace.str();
}

std::vector<std::string> SfException::stackTrace() const {
  return std::vector<std::string>(this->trace);
}

const char* SfException::what() const throw() {
  return this->message.c_str();
}


ErrNoException::ErrNoException(std::string message) : SfException(message) {
  std::stringstream formatter;
  this->error_number = errno;

  formatter << this->message << " " << strerror(this->error_number);
  formatter << " (" << this->error_number << ").";
  this->message = formatter.str();
}

int ErrNoException::getCode() const {
  return this->error_number;
}


SocketException::SocketException(int code, std::string message) : SfException(
    message
) {
  this->code = code;
}

int SocketException::getCode() const {
  return -3000 * this->code;
}


UnrecognisedEvent::UnrecognisedEvent(std::string event) : SfException(
    "Received unrecognised event '" + event + "'"
) { }

int UnrecognisedEvent::getCode() const {
  return -1;
}


MSG_DEFINITION(SfException, CorruptedData, -10);
MSG_DEFINITION(SfException, ContextUninitialised, -23);
MSG_DEFINITION(SfException, DuplicateInjection, -18);
MSG_DEFINITION(SfException, FactoryNotFound, -24);
MSG_DEFINITION(SfException, FileError, -11);
MSG_DEFINITION(SfException, GroupNotFound, -12);
MSG_DEFINITION(SfException, InvalidCommandLine, -13);
NO_ARG_DEFINITION(SfException, InvalidDaemonSession, -14, "setsid failed.");
MSG_DEFINITION(SfException, InvalidValueOperation, -15);
MSG_DEFINITION(SfException, NotImplemented, -16);
MSG_DEFINITION(SfException, ServiceNotFound, -20);
MSG_DEFINITION(SfException, TypeError, -25);
MSG_DEFINITION(SfException, UserNotFound, -17);

NO_ARG_DEFINITION(
    SfException, ChannelEmpty, -19, "No data could be read from the channel."
);
NO_ARG_DEFINITION(
    SfException, CleanExit, 0, "Process terminating successfully."
);
NO_ARG_DEFINITION(
    SfException, ForkFailed, -2, "Unable to fork a new process."
);
NO_ARG_DEFINITION(
    SfException, ImpossiblePath, -3,
    "An invalid system state has been reached."
);
NO_ARG_DEFINITION(
    SfException, OperationNotPermitted, -22,
    "The requested operation is not allowed within the context."
);
NO_ARG_DEFINITION(
    SfException, StopException, -26,
    "Handling of the event stopped."
);
NO_ARG_DEFINITION(
    SfException, UnsupportedMode, -4,
    "Attempted to start SnowFoxDaemon with an unsupported mode."
);
