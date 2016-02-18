// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/drain/fd.h"

#include <string>

#include "core/context/static.h"
#include "core/model/event.h"


using sf::core::context::Static;
using sf::core::event::FdDrain;
using sf::core::model::EventDrain;


FdDrain::FdDrain(int fd, std::string id) : EventDrain("fd-" + id) {
  this->fd = fd;
}

FdDrain::~FdDrain() {
  Static::posix()->close(this->fd);
}

int FdDrain::getFD() {
  return this->fd;
}
