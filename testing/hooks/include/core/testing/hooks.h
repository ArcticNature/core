// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_TESTING_HOOKS_H_
#define CORE_TESTING_HOOKS_H_

#include <gtest/gtest.h>

#include <functional>
#include <vector>


namespace sf {
namespace core {
namespace testing {

  //! Test class to resets hooks before and after tests.
  class HookTest : public ::testing::Test {
   protected:
    std::vector<std::function<void(void)>> cleaners_;

    //! Clear tracked hooks.
    void clearHooks();

    //! Track a hook for clearing.
    template <typename Hook>
    void trackHook(Hook* hook) {
      auto cleaner = [hook] {
        hook->clear();
      };
      this->cleaners_.push_back(cleaner);
    }
  };


}  // namespace testing
}  // namespace core
}  // namespace sf

#endif  // CORE_TESTING_HOOKS_H_
