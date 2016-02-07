// Copyright 2016 Stefano Pogliani
#include "core/utility/daemoniser.h"

#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <string>

#include "core/context/static.h"
#include "core/exceptions/base.h"

#define DEFAULT_PATH "/usr/bin:/bin:/usr/sbin:/sbin:"


using sf::core::context::Static;
using sf::core::exception::GroupNotFound;
using sf::core::exception::InvalidDaemonSession;
using sf::core::exception::UserNotFound;

using sf::core::utility::Daemoniser;
using sf::core::utility::EnvMap;


int Daemoniser::child() {
  // Child can proceed past fork.
  return 0;
}

int Daemoniser::parent() {
  // Parent can terminate.
  Static::posix()->exit(0);
}


void Daemoniser::becomeSessionLeader() {
  pid_t session = Static::posix()->setsid();
  if (session == -1) {
    throw InvalidDaemonSession();
  }
}

void Daemoniser::changeDirectory(std::string new_dir) {
  Static::posix()->chdir(new_dir.c_str());
}

void Daemoniser::daemonise(
    EnvMap* new_env, std::string stdin, std::string stdout,
    std::string stderr, std::string new_dir, std::string user,
    std::string group
) {
  this->sanitiseEnvironment(new_env);
  this->detatchFromParentProcess();
  this->closeNonStdFileDescriptors();
  this->redirectStdFileDescriptors(stdin, stdout, stderr);
  this->changeDirectory(new_dir);
  this->dropPrivileges(user, group);
}

void Daemoniser::detatchFromParentProcess() {
  this->fork();
  this->becomeSessionLeader();
}

void Daemoniser::maskSigHup() {
  sigset_t mask;
  Static::posix()->sigemptyset(&mask);
  Static::posix()->sigaddset(&mask, SIGHUP);
  Static::posix()->sigprocmask(SIG_BLOCK, &mask, nullptr);
}

void Daemoniser::restoreSigHup() {
  sigset_t mask;
  Static::posix()->sigemptyset(&mask);
  Static::posix()->sigaddset(&mask, SIGHUP);
  Static::posix()->sigprocmask(SIG_UNBLOCK, &mask, nullptr);
}

void Daemoniser::sanitiseEnvironment(EnvMap* env) {
  // Deal with default vars.
  if (env == nullptr) {
    Static::posix()->clearenv();
    Static::posix()->setenv("PATH", DEFAULT_PATH, 1);
    return;
  }

  // Grab current value of needed vars.
  for (EnvMap::iterator it = env->begin(); it != env->end(); it++) {
    const char* key = it->first.c_str();
    if (it->second == "" && Static::posix()->getenv(key) != nullptr) {
      it->second = std::string(Static::posix()->getenv(it->first.c_str()));
    }
  }

  // Clean the environment.
  Static::posix()->clearenv();

  // Re-populate the environment from the map.
  if (env->find("PATH") == env->end()) {
    Static::posix()->setenv("PATH", DEFAULT_PATH, 1);
  }

  for (EnvMap::const_iterator it = env->begin(); it != env->end(); it++) {
    std::string key = it->first;
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    Static::posix()->setenv(key.c_str(), it->second.c_str(), 0);
  }
}
