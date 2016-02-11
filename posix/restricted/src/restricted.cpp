// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/posix/restricted.h"

#include <sys/wait.h>

#include "core/exceptions/base.h"


using sf::core::exception::OperationNotPermitted;
using sf::core::posix::Restricted;


Restricted::~Restricted() {}

int Restricted::kill(pid_t pid, int sig) {
  if (!(pid > 1 && (sig == SIGINT || sig == SIGTERM))) {
    throw OperationNotPermitted();
  }
  return ::kill(pid, sig);
}


pid_t Restricted::waitpid(pid_t pid, int* status, int options) {
  if (pid <= 1) {
    throw OperationNotPermitted();
  }
  return ::waitpid(pid, status, options);
}
