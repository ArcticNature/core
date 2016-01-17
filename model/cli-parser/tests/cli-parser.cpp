// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/model/cli-parser.h"

using sf::core::exception::ContextUninitialised;
using sf::core::exception::InvalidCommandLine;

using sf::core::model::CLIOption;
using sf::core::model::CLIOptionType;
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
  CLIParser::configOptions(&parser);
  CLIParser::daemonOptions(&parser);
  ASSERT_THROW(parser.parse(nullptr, nullptr), InvalidCommandLine);
}

TEST(CLIParser, checkRequiredOptionsPasses) {
  ValidParser parser;
  parser.parse(nullptr, nullptr);
  ASSERT_EQ("/path/to/repo", parser.getString("repo-path"));
}

TEST(CLIParser, SetDefaults) {
  NullParser parser;
  CLIParser::configOptions(&parser);
  CLIParser::daemonOptions(&parser);

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


class NullOption : public CLIOption {
 protected:
  bool _validate() {
    return true;
  }

 public:
  NullOption(CLIOptionType type, std::string name, std::string description) :
    CLIOption(type, name, description) {}

  void setDefault() {
    CLIOption::setDefault();
    this->parser->setBoolean("test", true);
  }
};


TEST(CLIOption, FailsWithNoParser) {
  NullOption option(CLIOptionType::CLIT_BOOL, "test", "false");
  ASSERT_THROW(option.setDefault(), ContextUninitialised);
  ASSERT_THROW(option.validate(), ContextUninitialised);
}

TEST(CLIOption, MetadataIsReturned) {
  NullOption option(CLIOptionType::CLIT_BOOL, "test", "false");
  ASSERT_EQ("false", option.description());
  ASSERT_EQ("test", option.name());
  ASSERT_EQ(CLIOptionType::CLIT_BOOL, option.type());
}

TEST(CLIOption, SetDefault) {
  NullOption option(CLIOptionType::CLIT_BOOL, "test", "false");
  NullParser parser;
  option.setParser(&parser);
  option.setDefault();
  ASSERT_TRUE(parser.getBoolean("test"));
}

TEST(CLIOption, ValidateIsCalled) {
  NullOption option(CLIOptionType::CLIT_BOOL, "test", "false");
  NullParser parser;
  option.setParser(&parser);
  ASSERT_TRUE(option.validate());
}
