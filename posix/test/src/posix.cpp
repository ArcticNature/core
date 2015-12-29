// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/posix/test.h"

using sf::core::posix::TestPosix;


TestPosix::TestPosix() {
  this->fork_child  = -1;
  this->fork_parent = -1;
  this->fork_result = -1;
}

pid_t TestPosix::fork() {
  return this->fork_result;
}

pid_t TestPosix::getpid() {
  return this->fork_child;
}

pid_t TestPosix::getppid() {
  return this->fork_parent;
}
