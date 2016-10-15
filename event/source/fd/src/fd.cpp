// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/fd.h"

#include <sys/socket.h>
#include <string>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/model/logger.h"


using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::FdSource;
using sf::core::event::ConnectedSource;
using sf::core::model::LogInfo;


void FdSource::disconnect() {
  LogInfo info = {{"source-id", this->id()}};
  INFOV(Context::Logger(), "Closing source ${source-id}", info);

  Static::posix()->shutdown(this->fd, SHUT_RD);
  Static::posix()->close(this->fd);
  this->fd = -1;
}

FdSource::FdSource(
    int fd, std::string id, std::string drain
) : ConnectedSource(id + "-fd", drain) {
  this->fd = fd;
}

FdSource::~FdSource() {
  this->safeCleanUp(false);
}

int FdSource::getFD() {
  return this->fd;
}
