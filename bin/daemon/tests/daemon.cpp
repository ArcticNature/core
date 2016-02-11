// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/bin/daemon.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/interface/posix.h"

using sf::core::bin::Daemon;
using sf::core::context::Static;
using sf::core::exception::ProcessNotFound;
using sf::core::interface::Posix;


class DaemonAccessor : public Daemon {
 public:
  std::string findManager() {
    return Daemon::findManager();
  }

  std::string findSpawner() {
    return Daemon::findSpawner();
  }
};


class PosixMock : public Posix {
 public:
  std::string binType = "manager";
  std::string find = "no";

  char* getcwd(char* buf, size_t size) {
    return strncpy(buf, "/test/path", size);
  }

  ssize_t readlink(const char* path, char* buf, size_t size) {
    strncpy(buf, "/test/path/exe", size);
    return 14;
  }

  int stat(const char* path, struct stat* buf) {
    std::string p(path);
    std::string debug = "/test/path/out/dist/debug/core/bin/";
    debug += this->binType + "/core.bin." + this->binType;

    if (this->find == "debug" && p == debug) {
      return 0;
    }

    if (this->find == "dist" && p == "/test/path/snow-fox-" + this->binType) {
      return 0;
    }

    errno = ENOENT;
    return -1;
  }
};


class DaemonTest : public ::testing::Test {
 protected:
  PosixMock* posix;

 public:
  std::string find;

  DaemonTest() {
    this->posix = new PosixMock();
    Static::initialise(this->posix);
  }
  ~DaemonTest() {
    Static::reset();
  }
};


TEST_F(DaemonTest, FindManagerFails) {
  DaemonAccessor daemon;
  ASSERT_THROW(daemon.findManager(), ProcessNotFound);
}

TEST_F(DaemonTest, FindManagerFoundDebug) {
  this->posix->binType = "manager";
  this->posix->find = "debug";

  DaemonAccessor daemon;
  std::string path = daemon.findManager();
  ASSERT_EQ(
      "/test/path/out/dist/debug/core/bin/manager/core.bin.manager",
      path
  );
}

TEST_F(DaemonTest, FindManagerFoundRelease) {
  this->posix->binType = "manager";
  this->posix->find = "dist";

  DaemonAccessor daemon;
  std::string path = daemon.findManager();
  ASSERT_EQ("/test/path/snow-fox-manager", path);
}

TEST_F(DaemonTest, FindSpawnerFails) {
  DaemonAccessor daemon;
  ASSERT_THROW(daemon.findSpawner(), ProcessNotFound);
}

TEST_F(DaemonTest, FindSpownerFoundDebug) {
  this->posix->binType = "spawner";
  this->posix->find = "debug";

  DaemonAccessor daemon;
  std::string path = daemon.findSpawner();
  ASSERT_EQ(
      "/test/path/out/dist/debug/core/bin/spawner/core.bin.spawner",
      path
  );
}

TEST_F(DaemonTest, FindSpawnerFoundRelease) {
  this->posix->binType = "spawner";
  this->posix->find = "dist";

  DaemonAccessor daemon;
  std::string path = daemon.findSpawner();
  ASSERT_EQ("/test/path/snow-fox-spawner", path);
}
