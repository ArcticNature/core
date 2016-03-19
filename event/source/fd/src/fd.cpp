// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/fd.h"

#include <sys/socket.h>
#include <string>

#include "core/context/static.h"
#include "core/model/event.h"


using sf::core::context::Static;
using sf::core::event::FdSource;
using sf::core::event::ConnectedSource;


void FdSource::disconnect() {
  Static::posix()->shutdown(this->fd, SHUT_RD);
  Static::posix()->close(this->fd);
}

FdSource::FdSource(
    int fd, std::string id, std::string drain
) : ConnectedSource(id + "-fd", drain) {
  this->fd = fd;
}

FdSource::~FdSource() {
  this->safeCleanUp(false);
  this->fd = -1;
}

int FdSource::getFD() {
  return this->fd;
}
