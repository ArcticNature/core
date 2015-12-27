// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/posix/user.h"

#include <fcntl.h>
#include <signal.h>
#include <sys/signalfd.h>

#include <gtest/gtest.h>

#include "core/exceptions/base.h"


using sf::core::exception::ErrNoException;
using sf::core::posix::User;


// Generic.
TEST(Posix, close_fail) {
  User posix;
  ASSERT_THROW(posix.close(-1), ErrNoException);
}

TEST(Posix, close_fail_silent) {
  User posix;
  ASSERT_EQ(-1, posix.close(-1, true));
}

TEST(Posix, close_ok) {
  char buffer;
  int  fd = open("/proc/self/status", O_RDONLY);
  User posix;

  // Make sure that the file was actually opened.
  int rsize = read(fd, &buffer, 1);
  ASSERT_EQ(1, rsize);
  ASSERT_EQ('N', buffer);

  posix.close(fd);
  rsize = read(fd, &buffer, 1);

  // Make sure that the file was actually closed.
  ASSERT_EQ(-1, rsize);
  ASSERT_EQ(EBADF,  errno);
}

TEST(Posix, read_fail) {
  char buffer;
  User posix;
  EXPECT_THROW(posix.read(-1, &buffer, 1), ErrNoException);
}

TEST(Posix, read_fail_silent) {
  char buffer;
  User posix;

  int res = posix.read(-1, &buffer, 1, true);
  ASSERT_EQ(-1, res);
  ASSERT_EQ(EBADF, errno);
}

TEST(Posix, read_ok) {
  char buffer;
  int  fd = open("/proc/self/status", O_RDONLY);
  User posix;

  int rsize = posix.read(fd, &buffer, 1);
  ASSERT_EQ(1, rsize);
  ASSERT_EQ('N', buffer);
  close(fd);
}


// Processes.
TEST(Posix, chdir) {
  User posix;
  char* current_directory = getcwd(nullptr, 0);

  posix.chdir("/");
  char* new_directory = getcwd(nullptr, 0);
  EXPECT_STREQ("/", new_directory);

  chdir(current_directory);
  delete current_directory;
  delete new_directory;
}

TEST(Posix, exit) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";

  User posix;
  ASSERT_EXIT(posix.exit(0), ::testing::ExitedWithCode(0), ".*");
  ASSERT_EXIT(posix.exit(2), ::testing::ExitedWithCode(2), ".*");
}

TEST(Posix, fork) {
  User posix;
  int result = posix.fork();
  if (result == 0) { exit(0); }  // Exit child.
  ASSERT_LT(0, result);          // Parent.
}

TEST(Posix, pids) {
  User posix;
  ASSERT_EQ(getpid(),  posix.getpid());
  ASSERT_EQ(getppid(), posix.getppid());
}


// Signal FD.
TEST(Posix, signalfd) {
  User posix;
  sigset_t  mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);

  posix.sigprocmask(SIG_BLOCK, &mask, nullptr);
  int fd = posix.signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);

  struct signalfd_siginfo signal_info;
  ssize_t                 signal_size;
  kill(getpid(), SIGUSR1);
  signal_size = read(fd, &signal_info, sizeof(struct signalfd_siginfo));
  
  EXPECT_EQ(sizeof(struct signalfd_siginfo), signal_size);
  EXPECT_EQ(SIGUSR1, signal_info.ssi_signo);
  close(fd);
}

TEST(Posix, signalfd_fail) {
  User posix;
  ASSERT_THROW(posix.signalfd(-2, nullptr, 0), ErrNoException);
}

TEST(Posix, sigprocmask_fail) {
  User posix;
  ASSERT_THROW(
      posix.sigprocmask(SIG_BLOCK | SIG_UNBLOCK, nullptr, (sigset_t*)1);,
      ErrNoException
  );
}
