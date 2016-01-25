// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/daemon.h"
#include "core/event/source/signal.h"
#include "core/exceptions/base.h"
#include "core/model/event.h"
#include "core/model/logger.h"

using sf::core::bin::DaemonSignalSource;
using sf::core::event::SignalSource;
using sf::core::exception::CleanExit;

using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::Logger;


class DaemonStop : public Event {
 public:
  explicit DaemonStop(std::string correlation) : Event(correlation, "NULL") {}
  void handle() {
    throw CleanExit();
  }
};


DaemonSignalSource::DaemonSignalSource() : SignalSource("daemon") {}

EventRef DaemonSignalSource::handleReloadConfig() {
  DEBUG(Logger::fallback(), "Requested configuration reload.");
  return EventRef();
}

EventRef DaemonSignalSource::handleState() {
  DEBUG(Logger::fallback(), "Requested system state.");
  return EventRef();
}

EventRef DaemonSignalSource::handleStop() {
  DEBUG(Logger::fallback(), "Requested system termination.");
  return EventRef(new DaemonStop("daemon-signal-0"));
}
