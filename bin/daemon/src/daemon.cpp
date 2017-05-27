// Copyright 2016 Stefano Pogliani
#include "core/bin/daemon.h"

#include <string>
#include <vector>

#include "core/compile-time/options.h"
#include "core/context/daemon.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/exceptions/base.h"
#include "core/model/logger.h"
#include "core/registry/event/managers.h"
#include "core/utility/process.h"

#define CWD_LEN 4096

using sf::core::bin::Daemon;
using sf::core::bin::DaemonSignalSource;
using sf::core::bin::DaemonToManagerSource;
using sf::core::bin::DaemonToSpawnerSource;

using sf::core::context::Context;
using sf::core::context::ProxyLogger;
using sf::core::context::Static;
using sf::core::exception::ProcessNotFound;

using sf::core::model::CLIParser;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManagerRef;

using sf::core::model::Logger;
using sf::core::model::LogInfo;
using sf::core::registry::LoopManager;

using sf::core::utility::SubProcess;
using sf::core::utility::SubProcessRef;
using sf::core::utility::processDirectory;


static ProxyLogger logger("core.bin.daemon.daemon");


void Daemon::cleanEnvironment() {
  DEBUG(logger, "Resetting environment.");
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
  INFO(logger, "Forking into daemon mode.");
  this->maskSigHup();
  this->detatchFromParentProcess();
}

void Daemon::dropUser() {
  INFO(logger, "Dropping privileges.");

  CLIParser*  parser = Static::parser();
  std::string group  = parser->getString("group");
  std::string user   = parser->getString("user");

  LogInfo info = { {"group", group}, {"user", user} };
  DEBUGV(
      logger,
      "Dropping privileges to user ${user} and group ${group}.",
      info
  );

  this->dropPrivileges(user, group);
}


void Daemon::createSockets() {
  CLIParser*  parser = Static::parser();
  std::string manager_path = parser->getString("manager-socket");
  std::string spawner_path = parser->getString("spawner-socket");

  // Link to Manager and Spawner.
  LoopManagerRef loop = Context::LoopManager();
  loop->add(EventSourceRef(new DaemonToManagerSource(manager_path)));
  loop->add(EventSourceRef(new DaemonToSpawnerSource(spawner_path)));
}

void Daemon::forkManager() {
  CLIParser*  parser = Static::parser();
  std::string path = this->findManager();
  SubProcessRef manager(new SubProcess(path));

  // Specify the parser to avoid compatibility issues.
  manager->appendArgument("--parser=gflags");

  // Cluster arguments.
  if (parser->hasString("node-name")) {
    manager->appendArgument("--node_name");
    manager->appendArgument(parser->getString("node-name"));
  }

  // Config arguments.
  manager->appendArgument("--repo_type");
  manager->appendArgument(parser->getString("repo-type"));
  manager->appendArgument("--repo_path");
  manager->appendArgument(parser->getString("repo-path"));
  manager->appendArgument("--repo_ver");
  manager->appendArgument(parser->getString("repo-version"));

  // IPC arguments.
  manager->appendArgument("--manager_socket");
  manager->appendArgument(parser->getString("manager-socket"));
  manager->appendArgument("--spawner_manager_socket");
  manager->appendArgument(parser->getString("spawner-manager-socket"));

  manager->run();
  sf::core::context::Daemon::instance()->setManager(manager);
}

void Daemon::forkSpawner() {
  CLIParser*  parser = Static::parser();
  std::string path = this->findSpawner();
  SubProcessRef spawner(new SubProcess(path));

  // Specify the parser to avoid compatibility issues.
  spawner->appendArgument("--parser=gflags");

  // Add arguments.
  spawner->appendArgument("--spawner_socket");
  spawner->appendArgument(parser->getString("spawner-socket"));
  spawner->appendArgument("--spawner_manager_socket");
  spawner->appendArgument(parser->getString("spawner-manager-socket"));

  spawner->run();
  sf::core::context::Daemon::instance()->setSpawner(spawner);

  // Make sure spawner initialisation is done first.
  Static::posix()->sleep(1);
}


void Daemon::configureEvents() {
  DEBUG(logger, "Setting up event subsystem.");
  this->registerDefaultSourceManager();
  LoopManagerRef loop = Context::LoopManager();
  loop->add(EventSourceRef(new DaemonSignalSource()));
}

void Daemon::disableSignals() {
  DEBUG(logger, "Disabling signals.");

  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGTERM);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);

#if TRAP_SIGINT
  DEBUG(logger, "Trapping SIGINT too.");
  sigaddset(&mask, SIGINT);
#endif

  Static::posix()->sigprocmask(SIG_BLOCK, &mask, nullptr);
}


std::string Daemon::findManager() {
  std::vector<std::string> paths;

  // When running from build the mode determines the location.
  char raw_cwd[CWD_LEN];
  std::string cwd(Static::posix()->getcwd(raw_cwd, CWD_LEN));
  std::string mode = DEBUG_BUILD ? "debug" : "release";
  paths.push_back(
      cwd + "/out/dist/" + mode +
      "/core/bin/manager/core.bin.manager"
  );

  // Production installations have the binaries in the same dir.
  std::string base = processDirectory();
  paths.push_back(base + "/snow-fox-manager");

  // Check with files exists.
  struct stat stat_info;
  std::vector<std::string>::iterator it;
  for (it = paths.begin(); it != paths.end(); it++) {
    if (Static::posix()->stat(it->c_str(), &stat_info) == 0) {
      return *it;
    }
  }

  throw ProcessNotFound("snow-fox-manager");
}


std::string Daemon::findSpawner() {
  std::vector<std::string> paths;

  // When running from build the mode determines the location.
  char raw_cwd[CWD_LEN];
  std::string cwd(Static::posix()->getcwd(raw_cwd, CWD_LEN));
  std::string mode = DEBUG_BUILD ? "debug" : "release";
  paths.push_back(
      cwd + "/out/dist/" + mode +
      "/core/bin/spawner/core.bin.spawner"
  );

  // Production installations have the binaries in the same dir.
  std::string base = processDirectory();
  paths.push_back(base + "/snow-fox-spawner");

  // Check with files exists.
  struct stat stat_info;
  std::vector<std::string>::iterator it;
  for (it = paths.begin(); it != paths.end(); it++) {
    if (Static::posix()->stat(it->c_str(), &stat_info) == 0) {
      return *it;
    }
  }

  throw ProcessNotFound("snow-fox-spawner");
}


void Daemon::initialise() {
  INFO(logger, "Initialising daemon.");

  // Prepare for process.
  CLIParser* parser = Static::parser();
  bool daemonise = parser->getBoolean("daemonise");
  bool drop_root = daemonise || parser->getBoolean("drop-privileges");

  this->cleanEnvironment();
  if (daemonise) {
    this->daemonise();
  }

  this->disableSignals();
  this->configureEvents();

  this->createSockets();
  this->forkSpawner();
  if (drop_root) {
    this->dropUser();
  }
  this->forkManager();
}
