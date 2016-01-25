// Copyright 2016 Stefano Pogliani
#include "core/bin/daemon.h"

#include <string>

#include "core/compile-time/options.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/exceptions/base.h"
#include "core/model/logger.h"
#include "core/registry/event/managers.h"

using sf::core::bin::Daemon;
using sf::core::bin::DaemonSignalSource;
using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::model::CLIParser;
using sf::core::model::EventSourceRef;
using sf::core::model::EventSourceManagerRef;

using sf::core::model::Logger;
using sf::core::model::LogInfo;
using sf::core::registry::EventSourceManager;


void Daemon::cleanEnvironment() {
  DEBUG(Logger::fallback(), "Resetting environment.");
  CLIParser*  parser = Static::parser();
  std::string stderr = parser->getString("stderr");
  std::string stdin  = parser->getString("stdin");
  std::string stdout = parser->getString("stdout");
  std::string work_dir = parser->getString("work-dir");

  this->sanitiseEnvironment(nullptr);
  this->closeNonStdFileDescriptors();
  this->redirectStdFileDescriptors(stdin, stdout, stderr);
  this->changeDirectory(work_dir);
}

void Daemon::daemonise() {
  INFO(Logger::fallback(), "Forking into daemon mode.");
  this->maskSigHup();
  this->detatchFromParentProcess();
}

void Daemon::dropUser() {
  INFO(Logger::fallback(), "Dropping privileges.");

  CLIParser*  parser = Static::parser();
  std::string group = parser->getString("group");
  std::string user = parser->getString("user");

  LogInfo info = { {"group", group}, {"user", user} };
  DEBUGV(
      Logger::fallback(),
      "Dropping privileges to user ${user} and group ${group}.",
      info
  );

  this->dropPrivileges(user, group);
}


void Daemon::createSocket(std::string path) {
  // TODO(stefano): Create UNIX socket for spawner and manager.
}

void Daemon::createSockets() {
  this->createSocket("/var/run/spawner.socket");
  this->createSocket("/var/run/manager.socket");
}

void Daemon::forkManager() {
  // TODO(stefano): Start manager.
}

void Daemon::forkSpawner() {
  // TODO(stefano): Fork spawner.
}


void Daemon::configureEvents() {
  DEBUG(Logger::fallback(), "Setting up event subsystem.");
  this->registerDefaultSourceManager();
  EventSourceManagerRef sources = Context::sourceManager();
  sources->addSource(EventSourceRef(new DaemonSignalSource()));
}

void Daemon::disableSignals() {
  DEBUG(Logger::fallback(), "Disabling signals.");

  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);

#if TRAP_SIGINT
  DEBUG(Logger::fallback(), "Trapping SIGINT too.");
  sigaddset(&mask, SIGINT);
#endif

  Static::posix()->sigprocmask(SIG_BLOCK, &mask, nullptr);
}


void Daemon::initialise() {
  INFO(Logger::fallback(), "Initialising daemon.");

  // Prepare for process.
  CLIParser* parser = Static::parser();
  bool daemonise = parser->getBoolean("daemonise");
  bool drop_root = daemonise || parser->getBoolean("drop-privileges");

  this->cleanEnvironment();
  if (daemonise) {
    this->daemonise();
  }

  this->disableSignals();
  this->createSockets();
  this->forkSpawner();
  if (drop_root) {
    this->dropUser();
  }

  this->configureEvents();
  this->forkManager();
}
