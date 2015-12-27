// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/options.h"
#include "core/model/cli-parser.h"


using sf::core::exception::VariableNotFound;
using sf::core::model::CLIParser;


class NullParser : public CLIParser {
 protected:
  void parseLogic(int* argc, char*** argv) {
    return;
  }
};

class ValidParser : public CLIParser {
 protected:
  void parseLogic(int* argc, char*** argv) {
    this->setString("repo-path", "/path/to/repo");
  }
};


TEST(CLIParser, checkRequiredOptionsFails) {
  NullParser parser;
  ASSERT_THROW(parser.parse(nullptr, nullptr), VariableNotFound);
}

TEST(CLIParser, checkRequiredOptionsPasses) {
  ValidParser parser;
  parser.parse(nullptr, nullptr);
  ASSERT_EQ("/path/to/repo", parser.getString("repo-path"));
}

TEST(CLIParser, setDefaults) {
  NullParser parser;

  // Daemon options.
  ASSERT_TRUE(parser.getBoolean("daemonise"));
  ASSERT_EQ("snow-fox", parser.getString("group"));
  ASSERT_EQ("snow-fox", parser.getString("user"));
  ASSERT_EQ("/dev/null", parser.getString("stderr"));
  ASSERT_EQ("/dev/null", parser.getString("stdin"));
  ASSERT_EQ("/dev/null", parser.getString("stdout"));
  ASSERT_EQ(".", parser.getString("work-dir"));

  // Misc options.
  ASSERT_EQ("git", parser.getString("repo-type"));
  ASSERT_EQ("<latest>", parser.getString("repo-version"));
}
