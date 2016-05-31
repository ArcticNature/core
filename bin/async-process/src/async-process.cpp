// Copyright 2016 Stefano Pogliani
#include "core/bin/async-process.h"

#include <unistd.h>
#include <string>

#include "core/compile-time/options.h"
#include "core/context/context.h"
#include "core/context/static.h"
#include "core/registry/event/managers.h"


using sf::core::bin::AsyncPorcess;
using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::exception::AbortException;
using sf::core::exception::CleanExit;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::SfException;

using sf::core::model::EventRef;
using sf::core::model::EventSourceManagerRef;
using sf::core::model::LogInfo;

using sf::core::registry::EventSourceManager;


void AsyncPorcess::disableSIGINT() {
  INFO(Context::logger(), "Disabling SIGINT.");

  sigset_t mask;
  Static::posix()->sigemptyset(&mask);
  Static::posix()->sigaddset(&mask, SIGINT);

  Static::posix()->sigprocmask(SIG_BLOCK, &mask, nullptr);
}

void AsyncPorcess::handleLoopError(
    EventRef event, SfException* ex, bool drain
) {
  if (event && drain) {
    try {
      // TODO(stefano): Find drain to which to send error information.
    } catch (SfException& drain_ex) {
      this->handleLoopError(event, &drain_ex, false);
    }
  }

  LogInfo vars = {
    {"error", ex->what()}, {"trace", ex->getTrace()},
    {"drain_error", drain ? "yes" : "no" }
  };

  ERRORV(
      Context::logger(),
      "Error (ignored) during run loop. ${error}", vars
  );
}

void AsyncPorcess::loop() {
  while (true) {
    EventRef event;
    EventSourceManagerRef source = Context::sourceManager();
    if (!source) {
      throw ContextUninitialised("Need event source to run!");
    }

    try {
      event = source->wait();
      if (!event) { continue; }

      // Attempt to handle or rescue.
      try {
        event->handle();
      } catch(CleanExit&) {
        throw;
      } catch(SfException& ex) {
        event->rescue(&ex);
      }

    } catch (CleanExit&) {
      break;

    } catch (AbortException& ex) {
      this->handleLoopError(event, &ex, true);
      throw;

    } catch (SfException& ex) {
      this->handleLoopError(event, &ex, true);
    }
  }
}

void AsyncPorcess::registerDefaultSourceManager() {
  DEBUG(Context::logger(), "Configuring default event source manager.");

  EventSourceManagerRef manager;
  std::string manager_name = DEAFULT_EVENT_SOURCE_MANAGER;
  manager = EventSourceManager::instance()->get(manager_name)();

  Context::instance()->initialise(manager);
}


void AsyncPorcess::run() {
  INFO(Context::logger(), "Starting run loop.");
  this->loop();
  INFO(Context::logger(), "Terminating cleanly.");
}
