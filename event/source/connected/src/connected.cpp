// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/connected.h"

#include <fcntl.h>
#include <string>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"

#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/utility/string.h"


using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::exception::ErrNoException;
using sf::core::exception::SfException;

using sf::core::event::ConnectedSource;
using sf::core::event::FileDescriptorState;

using sf::core::model::EventSource;
using sf::core::model::LogInfo;

using sf::core::model::EventSource;
using sf::core::utility::string::toString;


FileDescriptorState ConnectedSource::checkFdState(int fd) {
  try {
    char ch;
    int  read = Static::posix()->read(fd, &ch, 0);

    if (read == 0) {
      ssize_t size = Static::posix()->recv(
          fd, &ch, 1, MSG_DONTWAIT | MSG_PEEK
      );
      if (size == 0) {
        return FileDescriptorState::CLOSED;
      }
      return FileDescriptorState::OK;
    }

    return FileDescriptorState::UNKOWN;

  } catch (ErrNoException& ex) {
    switch (ex.getCode()) {
      case EAGAIN:   return FileDescriptorState::OK;
      case EBADF:    return FileDescriptorState::CLOSED;
      case ENOTSOCK: return FileDescriptorState::NOT_SOCKET;
    }
    return FileDescriptorState::UNKOWN;
  }
}


bool ConnectedSource::checkFD() {
  int fd = this->getFD();
  FileDescriptorState state = ConnectedSource::checkFdState(fd);
  switch (state) {
    case FileDescriptorState::CLOSED:
      this->onclose();
      return false;

    case FileDescriptorState::ERROR:
    case FileDescriptorState::UNKOWN:
      this->onerror();
      return false;
  }
  return true;
}

void ConnectedSource::onclose() {
  this->disconnect();
  this->removeSource();
  this->removeDrain();
}

void ConnectedSource::onerror() {
  ErrNoException last_error("");
  LogInfo info = {
    {"code",  toString(last_error.getCode())},
    {"trace", last_error.getTrace()},
    {"message",   last_error.what()},
    {"source-id", this->id()}
  };
  ERRORV(
      Context::logger(),
      "A ConnectedSource encountered an error: ${message}",
      info
  );

  try {
    this->disconnect();
  } catch (SfException& ex) {
    LogInfo info = {
      {"code",  toString(ex.getCode())},
      {"trace", ex.getTrace()},
      {"message",   ex.what()},
      {"source-id", this->id()}
    };
    ERRORV(
        Context::logger(),
        "Unable to disconnect source: ${message}",
        info
    );
  }
  this->removeSource();
  this->removeDrain();
}

void ConnectedSource::removeDrain() {
  Static::drains()->remove(this->drain_id);
}

void ConnectedSource::removeSource() {
  Context::sourceManager()->removeSource(this->id());
}

void ConnectedSource::safeCleanUp(bool self) {
  // If the fd is -1, it was closed while parsing.
  if (this->getFD() == -1) {
    return;
  }

  // Close connection to the client.
  try {
    this->disconnect();
  } catch (SfException& ex) {
    LogInfo info = {
      {"code",  toString(ex.getCode())},
      {"trace", ex.getTrace()},
      {"message",   ex.what()},
      {"source-id", this->id()}
    };
    ERRORV(
        Context::logger(),
        "Unable to disconnect source: ${message}",
        info
    );
  }

  // Remove source.
  try {
    if (self) {
      this->removeSource();
    }
  } catch (SfException& ex) {
    LogInfo info = {
      {"code",  toString(ex.getCode())},
      {"trace", ex.getTrace()},
      {"message",   ex.what()},
      {"source-id", this->id()}
    };
    ERRORV(
        Context::logger(),
        "Unable to remove source: ${message}",
        info
    );
  }

  // Remove drain.
  try {
    this->removeDrain();
  } catch (SfException& ex) {
    LogInfo info = {
      {"code",  toString(ex.getCode())},
      {"trace", ex.getTrace()},
      {"message",   ex.what()},
      {"drain-id",  this->drain_id},
      {"source-id", this->id()}
    };
    ERRORV(
        Context::logger(),
        "Unable to remove drain: ${message}",
        info
    );
  }
}

ConnectedSource::ConnectedSource(
    std::string id, std::string drain
) : EventSource(id), drain_id(drain) {}
