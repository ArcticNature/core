// Copyright 2016 Stefano Pogliani
#include <gtest/gtest.h>

#include "core/exceptions/base.h"

#include "core/context/daemon.h"
#include "core/context/static.h"

#include "core/posix/user.h"
#include "core/utility/subprocess.h"


using sf::core::exception::DuplicateInjection;

using sf::core::context::Static;
using sf::core::context::Daemon;
using sf::core::context::DaemonRef;

using sf::core::posix::User;
using sf::core::utility::SubProcess;
using sf::core::utility::SubProcessRef;


class DaemonContextTest : public ::testing::Test {
 public:
   DaemonContextTest() {
     Static::initialise(new User());
   }
  ~DaemonContextTest() {
    Daemon::destroy();
    Static::reset();
  }

  SubProcessRef mockProcess() {
    SubProcessRef proc(new SubProcess("echo"));
    proc->appendArgument("-n");
    return proc;
  }
};


TEST_F(DaemonContextTest, Singleton) {
  DaemonRef daemon1 = Daemon::instance();
  DaemonRef daemon2 = Daemon::instance();
  ASSERT_EQ(daemon1, daemon2);
}

TEST_F(DaemonContextTest, SetManagerTwiceIsNotAllowed) {
  DaemonRef daemon = Daemon::instance();
  daemon->setManager(this->mockProcess());
  ASSERT_THROW(daemon->setManager(nullptr), DuplicateInjection);
}

TEST_F(DaemonContextTest, SetSpawnerTwiceIsNotAllowed) {
  DaemonRef daemon = Daemon::instance();
  daemon->setSpawner(this->mockProcess());
  ASSERT_THROW(daemon->setSpawner(nullptr), DuplicateInjection);
}
