// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/interface/posix.h"
#include "core/utility/forker.h"

#include "core/posix/test.h"

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
