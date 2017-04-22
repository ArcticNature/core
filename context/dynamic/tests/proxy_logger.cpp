// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <vector>

#include "core/context/context.h"


using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::context::ProxyLogger;

using sf::core::model::LogInfo;
using sf::core::model::LogLevel;
using sf::core::model::Logger;


class NoopProxyLogger : public Logger {
 public:
  std::vector<LogInfo> logs;

  NoopProxyLogger() : Logger("") {}
  void log(
      const LogLevel level, const std::string message,
      LogInfo variables
  ) {
    this->logs.push_back(variables);
  }
};


class ProxyLoggerTest : public ::testing::Test {
 protected:
  ContextRef context;
  std::shared_ptr<NoopProxyLogger> logger;
  ProxyLogger proxy;

 public:
  ProxyLoggerTest() : proxy("proxy.logger.test") {
    this->logger  = std::make_shared<NoopProxyLogger>();
    this->context = ContextRef(new Context());
    this->context->initialise(this->logger);
    Context::Initialise(this->context);
  }

  ~ProxyLoggerTest() {
    this->logger.reset();
    Context::Destroy();
  }
};


TEST_F(ProxyLoggerTest, ForwardLogs) {
  this->proxy.log(LogLevel::LL_INFO, "ABC", {});
  ASSERT_EQ(1, this->logger->logs.size());
}

TEST_F(ProxyLoggerTest, SetComponent) {
  this->proxy.log(LogLevel::LL_INFO, "ABC", {});
  ASSERT_EQ(1, this->logger->logs.size());

  LogInfo info = this->logger->logs[0];
  LogInfo expected = {{"<component>", "proxy.logger.test"}};
  ASSERT_EQ(expected, info);
}

TEST_F(ProxyLoggerTest, OverwriteComponent) {
  this->proxy.log(LogLevel::LL_INFO, "ABC", {{"<component>", "c"}});
  ASSERT_EQ(1, this->logger->logs.size());

  LogInfo info = this->logger->logs[0];
  LogInfo expected = {{"<component>", "proxy.logger.test"}};
  ASSERT_EQ(expected, info);
}

TEST_F(ProxyLoggerTest, ArrowOperatorReturnsSelf) {
  ProxyLogger* ptr1 = &this->proxy;
  ProxyLogger* ptr2 = this->proxy.operator->();
  ASSERT_EQ(ptr1, ptr2);
}
