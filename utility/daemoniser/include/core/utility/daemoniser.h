// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_DAEMONISER_H_
#define CORE_UTILITY_DAEMONISER_H_

#include <sys/types.h>

#include <map>
#include <string>

#include "core/interface/posix.h"
#include "core/utility/forker.h"


namespace sf {
namespace core {
namespace utility {

  //! Type of high-order version of the environment map.
  typedef std::map<std::string, std::string> EnvMap;


  //! Utility class to daemonise processes.
  /*!
   * The daemonisation process is non-trivial process with several common
   * steps and that sometimes may require additional custom steps.
   * 
   * For this reason each step is provided independently and a daemonise
   * method calls them in the most common sequence.
   * 
   * This process is based on http://www.enderunix.org/docs/eng/daemon.php
   * with changes inspired by Secure Programming Cookbook for C and C++.
   */
  class Daemoniser : protected Forker {
   protected:
    virtual int child();
    virtual int parent();

   public:
    //! Makes the software the leader process for its current session.
    void becomeSessionLeader();

    //! Changes the current working directory to a safe location.
    void changeDirectory(std::string new_dir = "/");

    //! Closes all open file descriptors except for stdin, stdout and stderr.
    void closeNonStdFileDescriptors();

    //! Performs the standard daemonisation sequence.
    /*!
     * The daemonisaztion sequence is the following:
     *  1 - Sanitise environment
     *  2 - Fork and become independent
     *  3 - Close all file descriptors except 1, 2, 3
     *  4 - Point 1, 2, 3 to /dev/null
     *  5 - Change root
     *  6 - Drop privileges
     * 
     * @param new_env Map from environment variable to its value.
     * @param stdin   The path to the file to open for standard input.
     * @param stdout  The path to the file to open for standard output.
     * @param stderr  The path to the file to open for standard error.
     * @param user    The user name to drop to.
     * @param group   The group name to drop to.
     */
    virtual void daemonise(
        EnvMap* new_env, std::string stdin = "/dev/null",
        std::string stdout = "/dev/null", std::string stderr = "/dev/null",
        std::string new_dir = "/", std::string user = "nobody",
        std::string group = "nobody"
    );

    //! Detaches the process from the parent and becomes child of init.
    void detatchFromParentProcess();

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

    //! Disable handling of the SIGHUP signal.
    /*!
     * SIGHUP is sent to the process when the controlling process dies.
     * During daemonisation the process forks and the parent should die
     * while the child should become a session leader.
     * 
     * If the parent process dies before the child process has a chance
     * to become a session leader it will receive the SIGHUP signal and
     * terminate immediately.
     * 
     * This method masks SIGHUP so that the child process is not terminated.
     */
    void maskSigHup();

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

    //! Enables SIGHUP processing.
    void restoreSigHup();

    //! Clears all environment variables resetting them to a safe state.
    /*!
     * All environment variables are re-set to the values in the map.
     * Since environment variables are case insensitive but strings in
     * the map are case sensitive you may end up with clashes.
     * 
     * All variables are converted to UPPER case when the new
     * environment is build but it is up to you to keep variables unique.
     * 
     * If the map does not include the PATH variable the default value
     * of "/usr/bin:/bin:/usr/sbin:/sbin:" is used.
     * An empty value string indicates that a variable should be carried over.
     * 
     * @param env The new environment to use.
     */
    void sanitiseEnvironment(EnvMap* env = nullptr);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_DAEMONISER_H_
