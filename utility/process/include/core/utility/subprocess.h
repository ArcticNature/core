// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_SUBPROCESS_H_
#define CORE_UTILITY_SUBPROCESS_H_

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

    int child();
    int parent();

   public:
    explicit SubProcess(std::string binary);

    //! Appeds an argument to the command to run.
    void appendArgument(std::string argument);

    //! Runs the process and moves on.
    void run();

    //! Runs the process and waits for it to complete.
    int wait();
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_SUBPROCESS_H_
