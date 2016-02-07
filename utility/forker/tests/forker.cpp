// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/interface/posix.h"
#include "core/utility/forker.h"

#include "core/posix/test.h"

#define FIXTURE_BASE "core/utility/forker/tests/fixtures/"
#define FIXTURE_PATH(partial) FIXTURE_BASE partial

using sf::core::context::Static;
using sf::core::exception::ForkFailed;
using sf::core::interface::Posix;
using sf::core::utility::Forker;

using sf::core::posix::TestPosix;


class DummyForker : public Forker {
 protected:
  virtual int child() {
    return 0;
  }

  virtual int parent() {
    return 1;
  }

 public:
  void closeNonStdFileDescriptors() {
    Forker::closeNonStdFileDescriptors();
  }

  void dropPrivileges(
      std::string user = "nobody", std::string group = "nobody"
  ) {
    Forker::dropPrivileges(user, group);
  }
  
  void dropPrivileges(uid_t user = 1, gid_t group = 1) {
    Forker::dropPrivileges(user, group);
  }
  
  void redirectStdFileDescriptors(
      std::string stdin = "/dev/null", std::string stdout = "/dev/null",
      std::string stderr = "/dev/null"
  ) {
    Forker::redirectStdFileDescriptors(stdin, stdout, stderr);
  }
};


class ForkerTest : public ::testing::Test {
 protected:
  TestPosix* posix;

  ForkerTest() {
    this->posix = new TestPosix();
    Static::initialise(this->posix);
  }

  ~ForkerTest() {
    Static::reset();
  }
};


TEST_F(ForkerTest, CloseFDs) {
  std::string fixture = FIXTURE_PATH("streams.txt");
  DummyForker forker;

  FILE* handle = fopen(fixture.c_str(), "w"); 
  ASSERT_NE(nullptr, handle);

  ASSERT_EQ(fputc('A', handle), 'A');
  fflush(handle);

  forker.closeNonStdFileDescriptors();
  fputc('B', handle);
  fflush(handle);
  ASSERT_TRUE(ferror(handle));
}

// This test is usefull to bring attention to changes to the user
// dropping code, it does not really test it.
TEST_F(ForkerTest, DropUserGroupID) {
  DummyForker forker;
  forker.dropPrivileges(1, 2);

  ASSERT_TRUE(this->posix->setgroups_called);
  ASSERT_TRUE(this->posix->setegid_called);
  ASSERT_TRUE(this->posix->seteuid_called);
  ASSERT_TRUE(this->posix->setgid_called);
  ASSERT_TRUE(this->posix->setuid_called);
  ASSERT_TRUE(this->posix->setregid_called);
  ASSERT_TRUE(this->posix->setreuid_called);
}

TEST_F(ForkerTest, LookupRootRoot) {
  DummyForker forker;
  forker.dropPrivileges("root", "root");
  ASSERT_EQ(0, this->posix->drop_group);
  ASSERT_EQ(0, this->posix->drop_user);
}

TEST_F(ForkerTest, LookupRootBin) {
  DummyForker forker;
  forker.dropPrivileges("root", "bin");
  ASSERT_EQ(1, this->posix->drop_group);
  ASSERT_EQ(0, this->posix->drop_user);
}


TEST_F(ForkerTest, RunChild) {
  this->posix->fork_child  = 1;
  this->posix->fork_parent = 2;
  this->posix->fork_result = 0;

  DummyForker forker;
  int result = forker.fork();

  ASSERT_EQ(1, forker.getChildPid());
  ASSERT_EQ(2, forker.getParentPid());
  ASSERT_EQ(0, result);
}

TEST_F(ForkerTest, RunError) {
  this->posix->fork_child  = 1;
  this->posix->fork_parent = 2;
  this->posix->fork_result = -1;

  DummyForker forker;
  ASSERT_THROW(forker.fork(), ForkFailed);
}

TEST_F(ForkerTest, RunParent) {
  this->posix->fork_child  = 1;
  this->posix->fork_parent = 2;
  this->posix->fork_result = 2;

  DummyForker forker;
  int result = forker.fork();

  ASSERT_EQ(2, forker.getChildPid());
  ASSERT_EQ(1, forker.getParentPid());
  ASSERT_EQ(1, result);
}

TEST_F(ForkerTest, RedirectStdin) {
  DummyForker forker;
  forker.redirectStdFileDescriptors(FIXTURE_PATH("stdin.txt"), "", "");

  ASSERT_EQ(std::cin.get(), 'A');
  ASSERT_EQ(std::cin.get(), 'B');
  ASSERT_EQ(std::cin.get(), 'C');
  ASSERT_EQ(std::cin.get(), EOF);
  ASSERT_TRUE(std::cin.eof());
}
