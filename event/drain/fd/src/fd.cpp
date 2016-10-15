// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/drain/fd.h"

#include <sys/socket.h>
#include <string>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/model/event.h"
#include "core/model/logger.h"


using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::FdDrain;
using sf::core::model::EventDrain;
using sf::core::model::LogInfo;


FdDrain::FdDrain(int fd, std::string id) : EventDrain(id + "-fd") {
  this->fd = fd;
}

FdDrain::~FdDrain() {
  LogInfo info = {{"drain-id", this->id()}};
  INFOV(Context::Logger(), "Closing drain ${drain-id}", info);

  Static::posix()->shutdown(this->fd, SHUT_WR);
  Static::posix()->close(this->fd);
}

int FdDrain::getFD() {
  return this->fd;
}
