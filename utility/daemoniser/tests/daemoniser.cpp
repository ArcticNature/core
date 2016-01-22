// Copyright 2016 Stefano Pogliani
#include <stdlib.h>
#include <stdio.h>

#include <gtest/gtest.h>

#include <iostream>

#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/interface/posix.h"
#include "core/utility/daemoniser.h"

#define FIXTURE_BASE "core/utility/daemoniser/tests/fixtures/"
#define FIXTURE_PATH(partial) FIXTURE_BASE partial

using sf::core::context::Static;
using sf::core::exception::InvalidDaemonSession;
using sf::core::interface::Posix;

using sf::core::utility::Daemoniser;
using sf::core::utility::EnvMap;


class ExitTestException : public std::exception {};


class DaemonTestPosix : public Posix {
 public:
  int  fork_result   = -1;
  bool setsid_called = false;
  int  setsid_result = 0;

  bool chdir_called = false;

  bool setgroups_called = false;
  bool setegid_called = false;
  bool seteuid_called = false;
  bool setgid_called = false;
  bool setuid_called = false;
  bool setregid_called = false;
  bool setreuid_called = false;

  gid_t drop_group = -1;
  uid_t drop_user  = -1;

  virtual int chdir(const char* path) {
    this->chdir_called = true;
    EXPECT_STREQ("/some/path", path);
  }

  virtual void exit(int code) {
    EXPECT_EQ(0, code);
    throw ExitTestException();
  }

  virtual pid_t fork() {
    return this->fork_result;
  }

  virtual pid_t setsid() {
    this->setsid_called = true;
    return this->setsid_result;
  }

  int setgroups(int size, gid_t list[]) {
    this->setgroups_called = true;
    return 0;
  }

  int setegid(gid_t egid) {
    this->setegid_called = true;
    return 0;
  }

  int seteuid(uid_t euid) {
    this->seteuid_called = true;
    return 0;
  }

  int setgid(gid_t gid) {
    this->drop_group = gid;
    this->setgid_called = true;
    return 0;
  }

  int setuid(uid_t uid) {
    this->drop_user = uid;
    this->setuid_called = true;
    return 0;
  }

  int setregid(gid_t rgid, gid_t egid) {
    this->setregid_called = true;
    return 0;
  }

  int setreuid(uid_t ruid, uid_t euid) {
    this->setreuid_called = true;
    return 0;
  }
};


class DaemoniserTest : public ::testing::Test {
 protected:
  DaemonTestPosix* posix;

  DaemoniserTest() {
    this->posix = new DaemonTestPosix();
    Static::initialise(this->posix);
  }

  ~DaemoniserTest() {
    Static::reset();
  }
};


TEST_F(DaemoniserTest, ChangeDir) {
  Daemoniser daemoniser;
  daemoniser.changeDirectory("/some/path");
  ASSERT_TRUE(this->posix->chdir_called);
}

TEST_F(DaemoniserTest, CloseFDs) {
  std::string fixture = FIXTURE_PATH("daemonise.txt");
  Daemoniser daemoniser;

  FILE* handle = fopen(fixture.c_str(), "w"); 
  ASSERT_NE(nullptr, handle);

  ASSERT_EQ(fputc('A', handle), 'A');
  fflush(handle);

  daemoniser.closeNonStdFileDescriptors();
  fputc('B', handle);
  fflush(handle);
  ASSERT_TRUE(ferror(handle));
}

TEST_F(DaemoniserTest, DetachChild) {
  Daemoniser daemoniser;
  this->posix->fork_result = 0;  // == 0 to be the child.

  daemoniser.detatchFromParentProcess();
  ASSERT_TRUE(this->posix->setsid_called);
}

TEST_F(DaemoniserTest, DetachFail) {
  Daemoniser daemoniser;
  this->posix->fork_result   = 0;  // == 0 to be the child.
  this->posix->setsid_result = -1;

  ASSERT_THROW(daemoniser.detatchFromParentProcess(), InvalidDaemonSession);
}

TEST_F(DaemoniserTest, DetachParent) {
  Daemoniser daemoniser;
  this->posix->fork_result = 1;  // > 0 to be the parent.

  ASSERT_THROW(daemoniser.detatchFromParentProcess(), ExitTestException);
}

// This test is usefull to bring attention to changes to the user
// dropping code, it does not really test it.
TEST_F(DaemoniserTest, DropUserGroupID) {
  Daemoniser daemoniser;
  daemoniser.dropPrivileges(1, 2);

  ASSERT_TRUE(this->posix->setgroups_called);
  ASSERT_TRUE(this->posix->setegid_called);
  ASSERT_TRUE(this->posix->seteuid_called);
  ASSERT_TRUE(this->posix->setgid_called);
  ASSERT_TRUE(this->posix->setuid_called);
  ASSERT_TRUE(this->posix->setregid_called);
  ASSERT_TRUE(this->posix->setreuid_called);
}

TEST_F(DaemoniserTest, LookupRootRoot) {
  Daemoniser daemoniser;
  daemoniser.dropPrivileges("root", "root");
  ASSERT_EQ(0, this->posix->drop_group);
  ASSERT_EQ(0, this->posix->drop_user);
}

TEST_F(DaemoniserTest, LookupRootBin) {
  Daemoniser daemoniser;
  daemoniser.dropPrivileges("root", "bin");
  ASSERT_EQ(1, this->posix->drop_group);
  ASSERT_EQ(0, this->posix->drop_user);
}

TEST_F(DaemoniserTest, EnvEmpty) {
  Daemoniser daemoniser;
  setenv("TEST_VAR", "set", 1);

  daemoniser.sanitiseEnvironment();
  ASSERT_EQ(getenv("TEST_VAR"), nullptr);
}

TEST_F(DaemoniserTest, EnvCustomVars) {
  EnvMap map;
  Daemoniser daemoniser;
  setenv("TEST_VAR", "set", 1);

  map["A"] = "ABC";
  map["TEST_ENV"] = "DEF";
  daemoniser.sanitiseEnvironment(&map);

  ASSERT_EQ(std::string(getenv("A")), "ABC");
  ASSERT_EQ(std::string(getenv("TEST_ENV")), "DEF");
}

TEST_F(DaemoniserTest, EnvCarryVar) {
  EnvMap map;
  Daemoniser daemoniser;
  setenv("TEST_ENV", "set", 1);

  map["TEST_ENV"] = "";
  daemoniser.sanitiseEnvironment(&map);
  ASSERT_EQ(std::string(getenv("TEST_ENV")), "set");
}

TEST_F(DaemoniserTest, EnvDefaultPath) {
  Daemoniser daemoniser;
  daemoniser.sanitiseEnvironment();

  std::string path(getenv("PATH"));
  ASSERT_EQ(path, "/usr/bin:/bin:/usr/sbin:/sbin:");
}

TEST_F(DaemoniserTest, EnvOverridePath) {
  EnvMap map;
  Daemoniser daemoniser;

  map["PATH"] = "ABC";
  daemoniser.sanitiseEnvironment(&map);

  std::string path(getenv("PATH"));
  ASSERT_EQ(path, "ABC");
}

TEST_F(DaemoniserTest, RedirectStdin) {
  Daemoniser daemoniser;
  daemoniser.redirectStdFileDescriptors(FIXTURE_PATH("stdin.txt"), "", "");

  ASSERT_EQ(std::cin.get(), 'A');
  ASSERT_EQ(std::cin.get(), 'B');
  ASSERT_EQ(std::cin.get(), 'C');
  ASSERT_EQ(std::cin.get(), EOF);
  ASSERT_TRUE(std::cin.eof());
}
