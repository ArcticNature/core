// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include "core/bin/client.h"

#include "core/compile-time/options.h"
#include "core/context/context.h"
#include "core/context/static.h"
#include "core/event/source/signal.h"

#include "core/exceptions/base.h"
#include "core/model/event.h"
#include "core/model/logger.h"


using sf::core::bin::Client;
using sf::core::context::Context;
using sf::core::context::ProxyLogger;
using sf::core::context::Static;
using sf::core::event::SignalSource;

using sf::core::exception::CleanExit;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManagerRef;


static ProxyLogger logger("core.bin.cilent.signals");


class ClientSignalSource : public SignalSource {
 public:
  ClientSignalSource() : SignalSource("client") {}

  sigset_t getSignalsMask() {
    sigset_t mask;
    Static::posix()->sigemptyset(&mask);
    Static::posix()->sigaddset(&mask, SIGTERM);

#if TRAP_SIGINT
    DEBUG(logger, "Trapping SIGINT too.");
    Static::posix()->sigaddset(&mask, SIGINT);
#endif

    return mask;
  }

  EventRef handleStop() {
    INFO(logger, "Terminating client ...");
    throw CleanExit();
  }

  // NOOP all unused signals..
  EventRef handleReloadConfig() {
    return EventRef();
  }

  EventRef handleState() {
    return EventRef();
  }
};


void Client::maskSignals() {
  Context::LoopManager()->add(EventSourceRef(new ClientSignalSource()));
}
