// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/daemon.h"
#include "core/context/daemon.h"
#include "core/context/static.h"

#include "core/model/event.h"
#include "core/model/logger.h"


using sf::core::bin::SigChild;
using sf::core::bin::TerminateDaemon;

using sf::core::context::Daemon;
using sf::core::context::DaemonRef;
using sf::core::context::Static;

using sf::core::model::Event;
using sf::core::model::Logger;


bool SigChild::checkChild(pid_t pid) {
  int status = 0;
  pid_t ret_pid = Static::posix()->waitpid(pid, &status, WNOHANG);

  DaemonRef context = sf::core::context::Daemon::instance();
  if (ret_pid != 0 && WIFEXITED(status)) {
    context->exiting(WEXITSTATUS(status));
    return true;
  }
  if (ret_pid != 0 && WIFSIGNALED(status)) {
    context->exiting(WTERMSIG(status));
    return true;
  }
  return false;
}


SigChild::SigChild(std::string correlation) : Event(correlation, "NULL") {
}

void SigChild::handle() {
  // Figure out which child died.
  DaemonRef context = sf::core::context::Daemon::instance();
  pid_t manager = context->managerPid();
  pid_t spawner = context->spawnerPid();

  if (manager != -1 && this->checkChild(manager)) {
    context->clearManager();
    ERROR(Logger::fallback(), "Manager terminated, exiting!");
  }

  if (spawner != -1 && this->checkChild(spawner)) {
    context->clearSpawner();
    ERROR(Logger::fallback(), "Spawner terminated, exiting!");
  }

  // Kick off system shutdown.
  TerminateDaemon terminate(this->correlation_id);
  terminate.handle();
}
