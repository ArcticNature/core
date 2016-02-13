// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/unix.h"


using sf::core::event::UnixSource;
using sf::core::model::EventSource;


UnixSource::UnixSource(std::string id) : EventSource("unix-" + id) {
  this->socket_fd = -1;
}

int UnixSource::getFD() {
  if (this->socket_fd == -1) {
    this->openSocket();
  }
  return this->socket_fd;
}
