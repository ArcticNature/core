// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/daemon.h"
#include "core/context/daemon.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"

#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/protocols/daemon_spanwer/ds_message.pb.h"
#include "core/utility/protobuf.h"
#include "core/utility/string.h"


using sf::core::bin::TerminateDaemon;
using sf::core::context::Static;

using sf::core::exception::CleanExit;
using sf::core::exception::SfException;

using sf::core::model::Event;
using sf::core::model::Logger;
using sf::core::model::LogInfo;

using sf::core::protocol::daemon_spanwer::Message;
using sf::core::utility::MessageIO;
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
  // TODO(stefano): replace this with sending a message.
  pid_t manager = this->context->managerPid();
  signalProcess(manager, SIGTERM);
}

void TerminateDaemon::stopSpawner() {
  WARNING(Logger::fallback(), "Stopping spawner.");

  Message message;
  message.set_code(Message::Shutdown);

  int drain = this->context->spawnerDrain()->getFD();
  MessageIO<Message>::send(drain, message);
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
