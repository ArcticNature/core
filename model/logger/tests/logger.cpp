// Copyright 2015 Stefano Pogliani
#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/model/logger.h"

using sf::core::context::Static;
using sf::core::model::LogLevel;
using sf::core::model::Logger;
using sf::core::model::LogInfo;


class TestLogger : public Logger {
 public:
  std::string message;
  LogLevel level;
  LogInfo  vars;

  TestLogger() : Logger("") {};
  TestLogger(std::string f) : Logger(f) {};

  std::string externalFormatMessage(
      const LogLevel level, const std::string message,
      LogInfo vars
  ) {
    return this->formatMessage(level, message, vars);
  };

  void log(
      const LogLevel level, const std::string message,
      LogInfo variables
  ) {
    this->level   = LogLevel(level);
    this->message = std::string(message);
    this->vars    = variables;
  };
};


TEST(LoggerTest, PopulateMacro) {
  LogInfo vars;
  vars["test"] = "set";

  LogInfo populated = LOG_ADD_VARS(vars);
  ASSERT_EQ(vars, populated);

  ASSERT_EQ(4, populated.size());
  ASSERT_EQ("set", populated["test"]);
  ASSERT_EQ("44",  populated["line"]);
  ASSERT_EQ("core/model/logger/tests/logger.cpp", populated["file"]);
  ASSERT_EQ(
      "virtual void LoggerTest_PopulateMacro_Test::TestBody()",
      populated["function"]
  );
}

TEST(LoggerTest, Log) {
  TestLogger logger;
  LogInfo    variables;

  LOG_ADD_VARS(variables);
  logger.log(LogLevel::LL_ERROR, "Test", variables);

  ASSERT_EQ(LogLevel::LL_ERROR, logger.level);
  ASSERT_EQ("Test", logger.message);
  ASSERT_EQ(3, logger.vars.size());
}

TEST(LoggerTest, ShortMacro) {
  TestLogger logger;
  INFO((&logger), "Test");

  ASSERT_EQ(LogLevel::LL_INFO, logger.level);
  ASSERT_EQ("Test", logger.message);
  ASSERT_EQ(3, logger.vars.size());
}

TEST(LoggerTest, LongMacro) {
  TestLogger logger;
  LogInfo    vars;
  vars["before"] = "value";

  ERRORV((&logger), "Test", vars);

  ASSERT_EQ(LogLevel::LL_ERROR, logger.level);
  ASSERT_EQ("Test", logger.message);
  ASSERT_EQ(4, logger.vars.size());
  ASSERT_EQ("83", logger.vars["line"]);
  ASSERT_EQ("value", logger.vars["before"]);
}

TEST(LoggerTest, Format) {
  TestLogger logger("${level}::${message} and some text.");
  LogInfo    vars;

  vars["a"] = "0";
  vars["b"] = "1";
  vars["c"] = "2";
  
  ASSERT_EQ(
      "INFO::A=>0 B=>1 C=>2 and some text.",
      logger.externalFormatMessage(
          LogLevel::LL_INFO, "A=>${a} B=>${b} C=>${c}", vars
      )
  );
}

TEST(LoggerTest, Unicode) {
  TestLogger logger("${level}::${message}::£");
  LogInfo    vars;

  vars["£a"] = "0£";
  vars["b£"] = "1£";
  vars["£c"] = "2£";
  
  ASSERT_EQ("INFO::A=>0£ B=>1£ C=>2£::£", logger.externalFormatMessage(
      LogLevel::LL_INFO, "A=>${£a} B=>${b£} C=>${£c}", vars
  ));
}


class StaticLoggerTest : public ::testing::Test {
 public:
  ~StaticLoggerTest() {
    Static::destroy();
  }
};


TEST_F(StaticLoggerTest, LogGroupMissing) {
  TestLogger logger("${<log-group>}::${level}::${message}");
  LogInfo    vars;

  ASSERT_EQ("Global::INFO::A", logger.externalFormatMessage(
      LogLevel::LL_INFO, "A", vars
  ));
}

TEST_F(StaticLoggerTest, LogGroupSet) {
  TestLogger logger("${<log-group>}::${level}::${message}");
  LogInfo    vars;
  Static::options()->setString("log-group", "Test");
  ASSERT_EQ("Test::INFO::A", logger.externalFormatMessage(
      LogLevel::LL_INFO, "A", vars
  ));
}
