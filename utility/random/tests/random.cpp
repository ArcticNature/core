// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/utility/random.h"


#define TEST_SEED 42


using sf::core::exception::InvalidArgument;
using sf::core::utility::IdentifierGenerator;


class IdentifierGeneratorTest : public ::testing::Test {
 protected:
  IdentifierGenerator generator_;

  IdentifierGeneratorTest() : generator_(TEST_SEED) {
    // NOOP.
  }
};


TEST_F(IdentifierGeneratorTest, AtLeastOneWord) {
  ASSERT_THROW(this->generator_.generate(0), InvalidArgument);
}

TEST_F(IdentifierGeneratorTest, OneWord) {
  std::string id = this->generator_.generate(1);
  ASSERT_EQ("c151df7d6ee5e2d6", id);
}

TEST_F(IdentifierGeneratorTest, TwoWord) {
  std::string id = this->generator_.generate(2);
  ASSERT_EQ("c151df7d6ee5e2d6a3978fb9b92502a8", id);
}

TEST_F(IdentifierGeneratorTest, ThreeWord) {
  std::string id = this->generator_.generate(3);
  ASSERT_EQ("c151df7d6ee5e2d6a3978fb9b92502a8c08c967f0e5e7b0a", id);
}
