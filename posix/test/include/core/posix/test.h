// Copyright Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_POSIX_TEST_H_
#define CORE_POSIX_TEST_H_

#include <string>

#include "core/interface/posix.h"


namespace sf {
namespace core {
namespace posix {

  //! Used to test the exiting of processes.
  class ExitTestException : public std::exception {};

  //! Mock posix instance to run tests agains.
  /*!
   * The following attributes can be set:
   *
   *   * fork_child:  set the return value of `getpid()`
   *   * fork_parent: set the return value of `getppid()`
   *   * fork_result: set the return value of `fork()`
   */
  class TestPosix : public sf::core::interface::Posix {
   public:
    TestPosix();

    bool chdir_called;
    std::string chdir_path;

    int  exit_code;
    bool exit_raise;

    pid_t fork_child;
    pid_t fork_parent;
    pid_t fork_result;

    bool setsid_called;
    int  setsid_result;

    bool setgroups_called;
    bool setegid_called;
    bool seteuid_called;
    bool setgid_called;
    bool setuid_called;
    bool setregid_called;
    bool setreuid_called;

    gid_t drop_group;
    uid_t drop_user;

    virtual int  chdir(const char* path);
    virtual void exit(int code);

    virtual pid_t fork();
    virtual pid_t getpid();
    virtual pid_t getppid();

    virtual pid_t setsid();
    virtual int setgroups(int size, gid_t list[]);
    virtual int setegid(gid_t egid);
    virtual int seteuid(uid_t euid);
    virtual int setgid(gid_t gid);
    virtual int setuid(uid_t uid);
    virtual int setregid(gid_t rgid, gid_t egid);
    virtual int setreuid(uid_t ruid, uid_t euid);
  };

}  // namespace posix
}  // namespace core
}  // namespace sf

#endif  // CORE_POSIX_TEST_H_
