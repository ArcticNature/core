// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/context.h"
#include "core/model/logger.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;

using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::EventSourceManager;
using sf::core::model::EventSourceManagerRef;
using sf::core::model::EventSourceRef;

using sf::core::model::Logger;
using sf::core::model::LoggerRef;
using sf::core::model::LogInfo;
using sf::core::model::LogLevel;


class ContextTest : public ::testing::Test {
 protected:
  ContextRef context;

 public:
  ContextTest() {
    this->context = ContextRef(new Context());
    Context::initialise(this->context);
  }
  ~ContextTest() {
    Context::reset();
    Logger::destroyFallback();
  }
};


class NoopLogger : public Logger {
 public:
  NoopLogger() : Logger("") {}

  void log(
      const LogLevel level, const std::string message,
      LogInfo variables
  ) {
    // Noop
  }
};


class NoopSourceManager : public EventSourceManager {
  public:
   void addSource(EventSourceRef source) {}
   void removeSource(std::string id) {}
   EventRef wait(int timeout = -1) {}
};


TEST_F(ContextTest, Defaults) {
  ASSERT_EQ(Logger::fallback(), Context::logger());
  ASSERT_EQ(nullptr, Context::sourceManager().get());
}

TEST_F(ContextTest, ReplaceActiveContext) {
  ContextRef context(new Context());
  ASSERT_NE(context, Context::instance());

  Context::initialise(context);
  ASSERT_EQ(context, Context::instance());
}

TEST_F(ContextTest, SetLogger) {
  LoggerRef logger(new NoopLogger());
  this->context->initialise(logger);
  ASSERT_EQ(logger, Context::logger());
}

TEST_F(ContextTest, SetSourceManager) {
  EventSourceManagerRef manager(new NoopSourceManager());
  this->context->initialise(manager);
  ASSERT_EQ(manager, Context::sourceManager());
}
