// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/fd.h"

#include <sys/socket.h>
#include <string>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/model/logger.h"


using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::FdSource;
using sf::core::event::ConnectedSource;
using sf::core::exception::ErrNoException;
using sf::core::model::EventDrainRef;
using sf::core::model::LogInfo;


void FdSource::disconnect() {
  LogInfo info = {{"source-id", this->id()}};
  INFOV(Context::Logger(), "Closing source ${source-id}", info);

  try {
    Static::posix()->shutdown(this->_fd, SHUT_RD);
    Static::posix()->close(this->_fd);
  } catch (ErrNoException& ex) {
    // Ignore bad file descriptors only.
    if (ex.getCode() != EBADF) {
      throw;
    }
  }
  this->_closed = true;
}

FdSource::FdSource(
    int fd, std::string id, EventDrainRef drain
) : ConnectedSource(id + "-fd", drain) {
  this->_fd = fd;
}

FdSource::~FdSource() {
  this->safeCleanUp(false);
}

int FdSource::fd() {
  return this->_fd;
}
