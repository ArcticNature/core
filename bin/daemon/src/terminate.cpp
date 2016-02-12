// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/daemon.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"

#include "core/model/event.h"
#include "core/model/logger.h"
#include "core/utility/string.h"


using sf::core::bin::TerminateDaemon;
using sf::core::context::Static;

using sf::core::exception::CleanExit;
using sf::core::exception::SfException;

using sf::core::model::Event;
using sf::core::model::Logger;
using sf::core::model::LogInfo;

using sf::core::utility::string::toString;


void signalProcess(pid_t pid, int signal) {
  try {
    Static::posix()->kill(pid, signal);

  } catch (SfException& ex) {
    LogInfo info;
    info["pid"] = toString(pid);
    info["signal"]  = toString(signal);
    info["message"] = ex.what();
    info["trace"]   = ex.getTrace();
    ERRORV(
        Logger::fallback(),
        "Failed to send signal ${signal} to pid ${pid}: ${message}",
        info
    );
  }
}


void TerminateDaemon::rudeStop() {
  WARNING(Logger::fallback(), "Forced termination of daemon.");
  pid_t manager = this->context->managerPid();
  pid_t spawner = this->context->spawnerPid();

  if (manager != -1) {
    INFO(Logger::fallback(), "Sigkilling manager.");
    signalProcess(manager, SIGKILL);
  }
  if (spawner != -1) {
    INFO(Logger::fallback(), "Sigkilling spawner.");
    signalProcess(spawner, SIGKILL);
  }

  throw CleanExit();
}

void TerminateDaemon::stopManager() {
  WARNING(Logger::fallback(), "Stopping manager.");
  pid_t manager = this->context->managerPid();
  signalProcess(manager, SIGTERM);
}

void TerminateDaemon::stopSpawner() {
  WARNING(Logger::fallback(), "Stopping spawner.");
  pid_t spawner = this->context->spawnerPid();
  signalProcess(spawner, SIGTERM);
}


TerminateDaemon::TerminateDaemon(std::string correlation) : Event(
    correlation, "NULL"
) {
  this->context = sf::core::context::Daemon::instance();
}

void TerminateDaemon::handle() {
  this->context->shutdown();

  // Determine if immediate shutdown is needed.
  if (this->context->terminatingForced()) {
    this->rudeStop();
    return;
  }

  // Determine the next shutdown phase.
  if (this->context->managerPid() != -1) {
    this->stopManager();
    return;
  }
  if (this->context->spawnerPid() != -1) {
    this->stopSpawner();
    return;
  }

  // When all subprocesses are dead we can terminate.
  throw CleanExit();
}
