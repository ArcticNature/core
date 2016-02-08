// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/posix/test.h"

using sf::core::posix::TestPosix;
using sf::core::posix::ExitTestException;


TestPosix::TestPosix() {
  this->chdir_path = "";
  this->chdir_called = false;

  this->exit_code = 0;
  this->exit_raise = false;

  this->execvp_args    = nullptr;
  this->execvp_binary  = "";
  this->execvp_called  = false;

  this->waitpid_called = false;
  this->waitpid_pid    = -1;

  this->fork_called = false;
  this->fork_child  = -1;
  this->fork_parent = -1;
  this->fork_result = -1;

  this->setsid_called = false;
  this->setsid_result = 0;

  this->setgroups_called = false;
  this->setegid_called = false;
  this->seteuid_called = false;
  this->setgid_called = false;
  this->setuid_called = false;
  this->setregid_called = false;
  this->setreuid_called = false;

  this->drop_group = -1;
  this->drop_user  = -1;
}

TestPosix::~TestPosix() {
  if (this->execvp_args != nullptr) {
    this->free((void*)this->execvp_args);
  }
}

void TestPosix::exit(int code) {
  EXPECT_EQ(this->exit_code, code);
  if (this->exit_raise) {
    throw ExitTestException();
  }
}

int TestPosix::execvp(const char* file, char* const argv[]) {
  if (this->execvp_args != nullptr) {
    this->free((void*)this->execvp_args);
  }

  this->execvp_args   = argv;
  this->execvp_binary = std::string(file);
  this->execvp_called = true;
  return 0;
}

int TestPosix::chdir(const char* path) {
  this->chdir_called = true;
  EXPECT_EQ(this->chdir_path, path);
}

pid_t TestPosix::fork() {
  this->fork_called = true;
  return this->fork_result;
}

pid_t TestPosix::waitpid(pid_t pid, int* status, int options) {
  this->waitpid_called = true;
  this->waitpid_pid = pid;
  *status = 1 << 8;
  return 0;
}

pid_t TestPosix::getpid() {
  return this->fork_child;
}

pid_t TestPosix::getppid() {
  return this->fork_parent;
}

pid_t TestPosix::setsid() {
  this->setsid_called = true;
  return this->setsid_result;
}

int TestPosix::setgroups(int size, gid_t list[]) {
  this->setgroups_called = true;
  return 0;
}

int TestPosix::setegid(gid_t egid) {
  this->setegid_called = true;
  return 0;
}

int TestPosix::seteuid(uid_t euid) {
  this->seteuid_called = true;
  return 0;
}

int TestPosix::setgid(gid_t gid) {
  this->drop_group = gid;
  this->setgid_called = true;
  return 0;
}

int TestPosix::setuid(uid_t uid) {
  this->drop_user = uid;
  this->setuid_called = true;
  return 0;
}

int TestPosix::setregid(gid_t rgid, gid_t egid) {
  this->setregid_called = true;
  return 0;
}

int TestPosix::setreuid(uid_t ruid, uid_t euid) {
  this->setreuid_called = true;
  return 0;
}
