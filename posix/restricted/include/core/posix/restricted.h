// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_POSIX_RESTRICTED_H_
#define CORE_POSIX_RESTRICTED_H_

#include "core/interface/posix.h"


namespace sf {
namespace core {
namespace posix {

  //! Posix interface used by the Spawner.
  /*!
   * This is a restricted set that provides restricted functionality.
   * 
   * Since the binary is the same for the whole daemon the regular Posix
   * interface lives in the address space accessible by the root process.
   * 
   * Despite this, using a dedicated interface with only the required
   * operations and that enforces additional constraints on the input
   * feels like a good idea as it would at least complicate exploiting
   * the system but it does not introduce much complexity in the code.
   */
  class Restricted : public sf::core::interface::Posix {
   public:
    virtual ~Restricted();

    // Process.
    virtual int   kill(pid_t pid, int sig);
    virtual pid_t waitpid(pid_t pid, int* status, int options);
  };

}  // namespace posix
}  // namespace core
}  // namespace sf

#endif  // CORE_POSIX_RESTRICTED_H_
