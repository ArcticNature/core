// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/scheduled.h"

#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/source/manual.h"

#include "core/interface/posix.h"
#include "core/model/logger.h"
#include "core/utility/string.h"

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::ManualSource;
using sf::core::event::ScheduledClosure;
using sf::core::event::ScheduledSource;

using sf::core::model::EventRef;
using sf::core::model::EventSource;
using sf::core::model::LogInfo;

using sf::core::utility::string::toString;


ScheduledSource::ScheduledSource(int tick, std::string id) : EventSource(id) {
  this->tick = tick;
  this->timer_fd = 0;
}

ScheduledSource::~ScheduledSource() {
  Static::posix()->close(this->timer_fd);
}

int ScheduledSource::getFD() {
  if (this->timer_fd == 0) {
    struct itimerspec interval  = {0};
    interval.it_interval.tv_sec = this->tick;
    interval.it_value.tv_sec    = this->tick;
    this->timer_fd = Static::posix()->timerfd_create(
        CLOCK_MONOTONIC, TFD_NONBLOCK
    );
    Static::posix()->timerfd_settime(this->timer_fd, 0, &interval, nullptr);
  }
  return this->timer_fd;
}

EventRef ScheduledSource::parse() {
  uint64_t count;
  Static::posix()->read(this->timer_fd, &count, sizeof(count));

  if (count > 1) {
    LogInfo info = {{"ticks", toString(count)}};
    WARNINGV(
        Context::logger(),
        "Multiple ticks (${ticks}) occurred before the Scheduler could "
        "catch up with them. This may indicate system overload.",
        info
    );
  }

  // Find events that need to be triggered.
  // If more than one event is found,
  // add them all to the manual source and return nullptr.
  // Oterwise return the one event or nullptr.
  bool single_return = true;
  EventRef maybe_return;
  ManualSource* manual = Context::sourceManager()->get<ManualSource>("manual");
  std::vector<ScheduledClosure> closures = this->closures.pop();
  std::vector<ScheduledClosure>::iterator it;

  for (it = closures.begin(); it != closures.end(); it++) {
    EventRef result = it->callback(it->closure);
    if (!result) {
      continue;
    }

    // A second event was returned.
    // Enqueue them both and return nullptr.
    if (single_return && maybe_return) {
      manual->enqueueEvent(maybe_return);
      manual->enqueueEvent(result);
      maybe_return  = EventRef();
      single_return = false;

    // The first event has been returned.
    } else if (single_return && !maybe_return) {
      maybe_return = result;

    // Any other event is immediately enqueued.
    } else if (!single_return) {
      manual->enqueueEvent(result);
    }
  }

  return maybe_return;
}

void ScheduledSource::registerCallback(
    unsigned int score, ScheduledClosure closure
) {
  this->closures.insert(score, closure);
}

void ScheduledSource::registerCallback(
    unsigned int score, scheduled_callback callback
) {
  ScheduledClosure closure;
  closure.callback = callback;
  closure.closure  = nullptr;
  this->registerCallback(score, closure);
}
