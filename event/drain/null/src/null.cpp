// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/drain/null.h"

#include <fcntl.h>

#include "core/context/static.h"
#include "core/model/event.h"


using sf::core::context::Static;
using sf::core::event::NullDrain;
using sf::core::model::EventDrain;


NullDrain::NullDrain() : EventDrain("NULL") {
  this->null_fd = -1;
}

NullDrain::~NullDrain() {
  if (this->null_fd != -1) {
    Static::posix()->close(this->null_fd);
  }
}

int NullDrain::getFD() {
  if (this->null_fd == -1) {
    this->null_fd = Static::posix()->open("/dev/null", O_WRONLY, 0);
  }
  return this->null_fd;
}

void NullDrain::sendAck() {
}


