// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/interface/posix.h"
#include "core/utility/subprocess.h"

#include "core/posix/test.h"

using sf::core::context::Static;
using sf::core::utility::SubProcess;

using sf::core::interface::Posix;
using sf::core::posix::TestPosix;


#define CHILD_PID  3
#define PARENT_PID 2


class SubProcessTest : public ::testing::Test {
 protected:
  TestPosix* posix;

  void makeChild() {
    this->posix->fork_result = 0;
    this->posix->fork_child  = CHILD_PID;
    this->posix->fork_parent = PARENT_PID;
  }

  void makeParent() {
    this->posix->fork_result = CHILD_PID;
    this->posix->fork_child  = PARENT_PID;
    this->posix->fork_parent = 1;
  }

 public:
  SubProcessTest() {
    this->posix = new TestPosix();
    Static::initialise(this->posix);
  }

  ~SubProcessTest() {
    Static::reset();
  }
};


TEST_F(SubProcessTest, Arguments) {
  this->makeChild();
  SubProcess cmd("ls");
  cmd.appendArgument("-la");
  cmd.appendArgument("--human");
  cmd.wait();
  cmd.appendArgument("--ignored");

  ASSERT_TRUE(this->posix->execvp_called);
  ASSERT_STREQ("ls", this->posix->execvp_args[0]);
  ASSERT_STREQ("-la", this->posix->execvp_args[1]);
  ASSERT_STREQ("--human", this->posix->execvp_args[2]);
  ASSERT_EQ(nullptr, this->posix->execvp_args[3]);
}

TEST_F(SubProcessTest, Binary) {
  this->makeChild();
  SubProcess cmd("ls");
  cmd.wait();

  ASSERT_TRUE(this->posix->execvp_called);
  ASSERT_STREQ("ls", this->posix->execvp_args[0]);
}

TEST_F(SubProcessTest, ClearSignals) {
  this->makeChild();
  SubProcess cmd("ls");
  cmd.wait();

  ASSERT_TRUE(this->posix->sigfillset_called);
  ASSERT_TRUE(this->posix->sigprocmask_called);
  ASSERT_EQ(this->posix->sigfillset_set, this->posix->sigprocmask_set);
}

TEST_F(SubProcessTest, RunChild) {
  this->makeChild();
  SubProcess cmd("ls");
  cmd.run();

  ASSERT_TRUE(this->posix->fork_called);
  ASSERT_TRUE(this->posix->execvp_called);
}

TEST_F(SubProcessTest, RunParent) {
  this->makeParent();
  SubProcess cmd("ls");
  cmd.run();

  ASSERT_TRUE(this->posix->fork_called);
  ASSERT_FALSE(this->posix->waitpid_called);
}

TEST_F(SubProcessTest, WaitChild) {
  this->makeChild();
  SubProcess cmd("ls");
  cmd.wait();

  ASSERT_TRUE(this->posix->fork_called);
  ASSERT_TRUE(this->posix->execvp_called);
}

TEST_F(SubProcessTest, WaitParent) {
  this->makeParent();
  SubProcess cmd("ls");
  int result = cmd.wait();

  ASSERT_TRUE(this->posix->fork_called);
  ASSERT_TRUE(this->posix->waitpid_called);
  ASSERT_EQ(CHILD_PID, this->posix->waitpid_pid);
  ASSERT_EQ(1, result);
}

// User & group.
// Redirects.


// Real subprocess tests
class RealSubProcessTest : public ::testing::Test {
 protected:
 public:
  RealSubProcessTest() {
    Static::initialise(new Posix());
  }

  ~RealSubProcessTest() {
    Static::reset();
  }
};


TEST_F(RealSubProcessTest, ExitError) {
  SubProcess cmd("cat");
  cmd.appendArgument("not a real file");
  int result = cmd.wait();
  ASSERT_EQ(1, result);
}

TEST_F(RealSubProcessTest, ExitSuccess) {
  SubProcess cmd("echo");
  cmd.appendArgument("-n");
  int result = cmd.wait();
  ASSERT_EQ(0, result);
}
//  I/O redirect.
