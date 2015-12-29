// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_FORKER_H_
#define CORE_UTILITY_FORKER_H_

#include <sys/types.h>


namespace sf {
namespace core {
namespace utility {

  //! Easily create child processes.
  /*!
   * Subclasses must implement the parent and child methods to define
   * the behaviours after the fork and simply call fork.
   * 
   * Keep in mind that the execution for both parent and child will resume
   * from after the call to fork unless something is done to prevent it.
   */
  class Forker {
   protected:
    pid_t child_pid = -1;   //!< Stores the PID of the child process.
    pid_t parent_pid = -1;  //!< Stores the PID of the parent process.

    //! What should the parent process do after fork.
    virtual int parent() = 0;

    //! What should the child process do after fork.
    virtual int child() = 0;

   public:
    //! Fork the current process and run child/parent methods.
    virtual int fork();

    //! Returns the PID of the child process.
    pid_t getChildPid() const;

    //! Returns the PID of the parent process.
    pid_t getParentPid() const;
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_FORKER_H_
