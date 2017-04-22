// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/daemon.h"
#include "core/context/daemon.h"
#include "core/context/context.h"
#include "core/event/source/signal.h"
#include "core/model/event.h"
#include "core/model/logger.h"

using sf::core::bin::DaemonSignalSource;
using sf::core::bin::SigChild;
using sf::core::bin::TerminateDaemon;

using sf::core::context::DaemonRef;
using sf::core::context::ProxyLogger;
using sf::core::event::SignalSource;

using sf::core::model::Event;
using sf::core::model::EventRef;


static ProxyLogger logger("core.bin.daemon.signal");


sigset_t DaemonSignalSource::getSignalsMask() {
  sigset_t mask = SignalSource::getSignalsMask();
  sigaddset(&mask, SIGCHLD);
  return mask;
}

EventRef DaemonSignalSource::handleSignal(int signo) {
  if (signo != SIGCHLD) {
    return SignalSource::handleSignal(signo);
  }
  return EventRef(new SigChild("daemon-signal-0"));
}


DaemonSignalSource::DaemonSignalSource() : SignalSource("daemon") {}

EventRef DaemonSignalSource::handleReloadConfig() {
  DEBUG(logger, "Requested configuration reload.");
  return EventRef();
}

EventRef DaemonSignalSource::handleState() {
  DEBUG(logger, "Requested system state.");
  return EventRef();
}

EventRef DaemonSignalSource::handleStop() {
  DaemonRef context = sf::core::context::Daemon::instance();
  if (context->terminating()) {
    context->shutdownForced();
  }

  WARNING(logger, "Requested system termination.");
  return EventRef(new TerminateDaemon("daemon-signal-0"));
}
