// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/manual.h"

#include <sys/eventfd.h>

#include "core/context/static.h"
#include "core/model/event.h"

using sf::core::context::Static;
using sf::core::event::ManualSource;

using sf::core::model::EventRef;
using sf::core::model::EventSource;


ManualSource::ManualSource() : EventSource("manual") {
  this->internal_queue_fd = 0;
}

ManualSource::~ManualSource() {
  Static::posix()->close(this->internal_queue_fd);
}

int ManualSource::getFD() {
  if (this->internal_queue_fd == 0) {
    this->internal_queue_fd = Static::posix()->eventfd(
        0, EFD_NONBLOCK | EFD_SEMAPHORE
    );
    eventfd_t value = this->internal_event_queue.size();
    Static::posix()->write(this->internal_queue_fd, &value, sizeof(value));
  }
  return this->internal_queue_fd;
}

void ManualSource::enqueueEvent(EventRef event) {
  eventfd_t value = 1;
  this->internal_event_queue.push(event);
  Static::posix()->write(this->internal_queue_fd, &value, sizeof(value));
}

EventRef ManualSource::parse() {
  eventfd_t value;
  Static::posix()->read(this->internal_queue_fd, &value, sizeof(value));
  EventRef event = this->internal_event_queue.front();
  this->internal_event_queue.pop();
  return event;
}
