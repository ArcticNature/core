// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_SUBPROCESS_H_
#define CORE_UTILITY_SUBPROCESS_H_

#include <memory>
#include <string>
#include <vector>

#include "core/utility/forker.h"

namespace sf {
namespace core {
namespace utility {

  //! Runs applications programmatically.
  class SubProcess : public Forker {
   protected:
    bool _wait;
    std::vector<std::string> arguments;
    std::string binary;

    std::string group;
    std::string user;

    std::string stderr;
    std::string stdin;
    std::string stdout;

    int child();
    int parent();

   public:
    explicit SubProcess(std::string binary);

    //! Appeds an argument to the command to run.
    void appendArgument(std::string argument);

    //! Drops privileges to the specified user/group.
    void impersonate(std::string user, std::string group = "");

    //! Redirects standard error output.
    void redirectErrorOutput(std::string path);

    //! Redirects standard input.
    void redirctStandardInput(std::string path);

    //! Redirects standard output.
    void redirctStandardOutput(std::string path);

    //! Waits for a subproces that was started with `run`.
    int join();

    //! Runs the process and moves on.
    void run();

    //! Runs the process and waits for it to complete.
    int wait();
  };

  //! Reference to a subprocess.
  typedef std::shared_ptr<SubProcess> SubProcessRef;

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_SUBPROCESS_H_
