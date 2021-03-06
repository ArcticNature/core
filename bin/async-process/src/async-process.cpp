// Copyright 2016 Stefano Pogliani
#include "core/bin/async-process.h"

#include <unistd.h>
#include <string>

#include "core/cluster/cluster.h"
#include "core/compile-time/options.h"

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/interface/metadata/store.h"
#include "core/registry/event/managers.h"


using sf::core::bin::AsyncPorcess;
using sf::core::cluster::Cluster;
using sf::core::cluster::ClusterRaw;

using sf::core::context::Context;
using sf::core::context::ProxyLogger;
using sf::core::context::Static;

using sf::core::exception::AbortException;
using sf::core::exception::CleanExit;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::SfException;

using sf::core::interface::MetaDataStore;
using sf::core::interface::MemoryMetadataStore;
//using sf::core::interface::NoMetadataStore;

using sf::core::model::EventRef;
using sf::core::model::LogInfo;
using sf::core::model::LoopManagerRef;

using sf::core::registry::LoopManager;


static ProxyLogger logger("core.bin.async-process");


void AsyncPorcess::configureInitialCluster() {
  // TODO(stefano): This is an in-memory store for speed.
  // TODO(stefano): After the configuration system swap with NoOp.
  Cluster cluster = std::make_shared<ClusterRaw>(
    std::make_shared<MemoryMetadataStore>()
    //std::make_shared<NoMetadataStore>()
  );
  Cluster::Instance(cluster);
}

void AsyncPorcess::disableSIGINT() {
  INFO(logger, "Disabling SIGINT.");

  sigset_t mask;
  Static::posix()->sigemptyset(&mask);
  Static::posix()->sigaddset(&mask, SIGINT);

  Static::posix()->sigprocmask(SIG_BLOCK, &mask, nullptr);
}

void AsyncPorcess::handleLoopError(
    EventRef event, std::exception_ptr exception, bool drain
) {
  if (event && drain) {
    try {
      // TODO(stefano): Find drain to which to send error information.
    } catch (SfException& drain_ex) {
      this->handleLoopError(event, std::current_exception(), false);
    }
  }

  try {
    std::rethrow_exception(exception);
  } catch(SfException& ex) {
    LogInfo vars = {
      {"error", ex.what()},
      {"trace", ex.getTrace()},
      {"drain_error", drain ? "yes" : "no" }
    };
    ERRORV(logger, "Error during run loop. ${error}", vars);
  }
}

void AsyncPorcess::loop() {
  while (true) {
    EventRef event;
    LoopManagerRef loop = Context::LoopManager();
    if (!loop) {
      throw ContextUninitialised("Need a LoopManager to run!");
    }

    try {
      event = loop->wait();
      if (!event) { continue; }

      // Attempt to handle or rescue.
      try {
        event->handle();
      } catch(CleanExit&) {
        throw;
      } catch(SfException& ex) {
        event->rescue(std::current_exception());
      }

    } catch (CleanExit&) {
      break;

    } catch (AbortException& ex) {
      this->handleLoopError(event, std::current_exception(), true);
      throw;

    } catch (SfException& ex) {
      this->handleLoopError(event, std::current_exception(), true);
    }
  }
}

void AsyncPorcess::registerDefaultSourceManager() {
  DEBUG(logger, "Configuring default event source manager.");

  LoopManagerRef manager;
  std::string manager_name = DEAFULT_EVENT_SOURCE_MANAGER;
  manager = LoopManager::instance()->get(manager_name)();

  Context::Instance()->initialise(manager);
}


void AsyncPorcess::run() {
  INFO(logger, "Starting run loop.");
  this->loop();
  INFO(logger, "Terminating cleanly.");
}
