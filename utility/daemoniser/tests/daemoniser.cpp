// Copyright 2016 Stefano Pogliani
#include <stdlib.h>
#include <stdio.h>

#include <gtest/gtest.h>

#include <iostream>

#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/interface/posix.h"
#include "core/utility/daemoniser.h"

#include "core/posix/test.h"

#define FIXTURE_BASE "core/utility/daemoniser/tests/fixtures/"
#define FIXTURE_PATH(partial) FIXTURE_BASE partial

using sf::core::context::Static;
using sf::core::exception::InvalidDaemonSession;
using sf::core::interface::Posix;

using sf::core::utility::Daemoniser;
using sf::core::utility::EnvMap;

using sf::core::posix::TestPosix;
using sf::core::posix::ExitTestException;


class DaemoniserTest : public ::testing::Test {
 protected:
  //DaemonTestPosix* posix;
  TestPosix* posix;

  DaemoniserTest() {
    //this->posix = new DaemonTestPosix();
    this->posix = new TestPosix();
    Static::initialise(this->posix);
  }

  ~DaemoniserTest() {
    Static::reset();
  }
};


TEST_F(DaemoniserTest, ChangeDir) {
  Daemoniser daemoniser;
  this->posix->chdir_path = "/some/path";
  daemoniser.changeDirectory("/some/path");
  ASSERT_TRUE(this->posix->chdir_called);
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
  this->posix->exit_code  = 0;
  this->posix->exit_raise = true;
  ASSERT_THROW(daemoniser.detatchFromParentProcess(), ExitTestException);
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
