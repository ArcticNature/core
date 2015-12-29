// Copyright Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_POSIX_TEST_H_
#define CORE_POSIX_TEST_H_

#include "core/interface/posix.h"


namespace sf {
namespace core {
namespace posix {

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

    pid_t fork_child;
    pid_t fork_parent;
    pid_t fork_result;

    virtual pid_t fork();
    virtual pid_t getpid();
    virtual pid_t getppid();
  };

}  // namespace posix
}  // namespace core
}  // namespace sf

#endif  // CORE_POSIX_TEST_H_
