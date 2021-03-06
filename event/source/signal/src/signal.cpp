// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/signal.h"

#include <string>

#include "core/compile-time/options.h"
#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"

#include "core/model/event.h"
#include "core/model/logger.h"
#include "core/utility/string.h"

using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::SignalSource;
using sf::core::exception::ErrNoException;

using sf::core::model::EventRef;
using sf::core::model::EventSource;
using sf::core::model::LogInfo;

using sf::core::utility::string::toString;


void SignalSource::createSignalFd() {
  sigset_t mask = this->getSignalsMask();
  Static::posix()->sigprocmask(SIG_BLOCK, &mask, nullptr);
  this->signal_fd = Static::posix()->signalfd(
      -1, &mask, SFD_NONBLOCK | SFD_CLOEXEC
  );
}

sigset_t SignalSource::getSignalsMask() {
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);

#if TRAP_SIGINT
  sigaddset(&mask, SIGINT);
#endif

  return mask;
}

EventRef SignalSource::handleSignal(int signo) {
  switch (signo) {
    case SIGINT:
    case SIGTERM: return this->handleStop();
    case SIGUSR1: return this->handleState();
    case SIGUSR2: return this->handleReloadConfig();
    default:
      LogInfo vars = {{"signal", toString(signo)}};
      ERRORV(
          Context::Logger(),
          "Received unrecognised signal ${signal}.",
          vars
      );
      return EventRef();
  }
}


SignalSource::SignalSource(std::string id) : EventSource("signal-" + id) {
  this->signal_fd = -1;
}

SignalSource::~SignalSource() {
  Static::posix()->close(this->signal_fd);
}

int SignalSource::fd() {
  if (this->signal_fd == -1) {
    this->createSignalFd();
  }
  return this->signal_fd;
}

EventRef SignalSource::parse() {
  struct signalfd_siginfo signal_info;
  ssize_t signal_size = Static::posix()->read(
      this->signal_fd, &signal_info,
      sizeof(struct signalfd_siginfo)
  );

  if (signal_size != sizeof(struct signalfd_siginfo)) {
    throw ErrNoException("Read error on signalfd:");
  }

  return this->handleSignal(signal_info.ssi_signo);
}
