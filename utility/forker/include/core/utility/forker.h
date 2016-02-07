// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_FORKER_H_
#define CORE_UTILITY_FORKER_H_

#include <sys/types.h>
#include <string>


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

    //! Closes all open file descriptors except for stdin, stdout and stderr.
    void closeNonStdFileDescriptors();

    //! Drops the privileges of the current user/group.
    /*!
     * @param user  The user name to drop to.
     * @param group The group name to drop to.
     */
    void dropPrivileges(
        std::string user = "nobody", std::string group = "nobody"
    );

    //! Drops the privileges of the current user/group.
    /*!
     * Similar to the string version but assumes that the arguments
     * are already the user ID and the group ID to which to drop.
     * 
     * \param user  The user id to drop to.
     * \param group The group id to drop to.
     */
    void dropPrivileges(uid_t user = 1, gid_t group = 1);

    //! Redirects stdin, stdout and stderr to safe files.
    /*!
     * @param stdin  The path to the file to open for standard input.
     * @param stdout The path to the file to open for standard output.
     * @param stderr The path to the file to open for standard error.
     */
    void redirectStdFileDescriptors(
        std::string stdin = "/dev/null", std::string stdout = "/dev/null",
        std::string stderr = "/dev/null"
    );

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
