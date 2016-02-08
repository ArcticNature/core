// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/context/static.h"
#include "core/model/cli-parser.h"
#include "core/posix/test.h"


using sf::core::context::Static;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::DuplicateInjection;

using sf::core::interface::Posix;
using sf::core::model::CLIParser;

using sf::core::posix::TestPosix;


class StaticTest : public ::testing::Test {
 protected:
  ~StaticTest() {
    Static::reset();
  }
};


class TestParser : public CLIParser {
 protected:
  void parseLogic(int* argc, char*** argv) {}
};


TEST_F(StaticTest, DoubleInitialiseFails) {
  Static::initialise(nullptr);
  ASSERT_THROW(Static::initialise(nullptr), DuplicateInjection);
}

TEST_F(StaticTest, NeedPosix) {
  ASSERT_THROW(Static::posix(), ContextUninitialised);
}

TEST_F(StaticTest, HasPosix) {
  Posix* posix = new TestPosix();
  Static::initialise(posix);
  ASSERT_EQ(posix, Static::posix());
}


TEST_F(StaticTest, DoubleParserFails) {
  CLIParser* parser = new TestParser();
  Static::parser(parser);
  ASSERT_THROW(Static::parser(parser), DuplicateInjection);
}

TEST_F(StaticTest, NeedParser) {
  ASSERT_THROW(Static::parser(), ContextUninitialised);
}

TEST_F(StaticTest, HasParser) {
  CLIParser* parser = new TestParser();
  Static::parser(parser);
  ASSERT_EQ(parser, Static::parser());
}