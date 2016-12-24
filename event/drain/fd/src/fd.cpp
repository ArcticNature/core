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
using sf::core::model::EventDrainBufferRef;
using sf::core::model::LogInfo;


FdDrain::FdDrain(int fd, std::string id) : EventDrain(id + "-fd") {
  this->_fd = fd;
}

FdDrain::~FdDrain() {
  LogInfo info = {{"drain-id", this->id()}};
  INFOV(Context::Logger(), "Closing drain ${drain-id}", info);

  try {
    Static::posix()->shutdown(this->_fd, SHUT_WR);
  } catch(...) {
    // Noop.
  }
  Static::posix()->close(this->_fd);
}

int FdDrain::fd() {
  return this->_fd;
}

bool FdDrain::flush() {
  if (this->buffer.empty()) {
    return true;
  }

  // Flush the top of the buffer.
  EventDrainBufferRef item = this->buffer[0];
  this->buffer.erase(this->buffer.begin());

  uint32_t size;
  char* data = item->remaining(&size);
  Static::posix()->write(this->fd(), data, size);
  return this->buffer.empty();
}
