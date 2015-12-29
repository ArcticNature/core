// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/forker.h"

#include <unistd.h>

#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/interface/posix.h"

using sf::core::context::Static;
using sf::core::exception::ForkFailed;
using sf::core::interface::Posix;
using sf::core::utility::Forker;


int Forker::fork() {
  pid_t pid = Static::posix()->fork();

  if (pid < 0) {  // Failed to fork.
    throw ForkFailed();

  } else if (pid == 0) {  // In child.
    this->child_pid  = Static::posix()->getpid();
    this->parent_pid = Static::posix()->getppid();
    return this->child();
  }

  // Else pid > 0 => In parent.
  this->child_pid  = pid;
  this->parent_pid = Static::posix()->getpid();
  return this->parent();
}

pid_t Forker::getChildPid() const {
  return this->child_pid;
}

pid_t Forker::getParentPid() const {
  return this->parent_pid;
}
