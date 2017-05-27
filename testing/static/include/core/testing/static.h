// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_TESTING_STATIC_H_
#define CORE_TESTING_STATIC_H_

#include <gtest/gtest.h>


namespace sf {
namespace core {
namespace testing {

  //! Test case that initialises and frees the Static context.
  class StaticContextTest : public ::testing::Test {
   protected:
    StaticContextTest();
    virtual ~StaticContextTest();
  };

}  // namespace testing
}  // namespace core
}  // namespace sf

#endif  // CORE_TESTING_STATIC_H_
