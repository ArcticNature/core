// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/fd.h"

#include <sys/socket.h>
#include <string>

#include "core/context/static.h"
#include "core/model/event.h"


using sf::core::context::Static;
using sf::core::event::FdSource;
using sf::core::model::EventSource;


FdSource::FdSource(int fd, std::string id) : EventSource("fd-" + id) {
  this->fd = fd;
}

FdSource::~FdSource() {
  Static::posix()->shutdown(this->fd, SHUT_RD);
  Static::posix()->close(this->fd);
}

int FdSource::getFD() {
  return this->fd;
}
