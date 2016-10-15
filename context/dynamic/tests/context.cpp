// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/context.h"
#include "core/exceptions/base.h"
#include "core/model/logger.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::exception::ContextUninitialised;

using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::LoopManager;
using sf::core::model::LoopManagerRef;
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
    Context::Initialise(this->context);
  }
  ~ContextTest() {
    Context::Destroy();
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


class NoopLoopManager : public LoopManager {
  public:
   void add(EventSourceRef source) {}
   void removeDrain(std::string id) {}
   void removeSource(std::string id) {}
   EventRef wait(int timeout = -1) {}
};


TEST_F(ContextTest, Defaults) {
  ASSERT_EQ(Logger::fallback(), Context::Logger());
  ASSERT_THROW(Context::LoopManager(), ContextUninitialised);
}

TEST_F(ContextTest, ReplaceActiveContext) {
  ContextRef context(new Context());
  ASSERT_NE(context, Context::Instance());

  Context::Initialise(context);
  ASSERT_EQ(context, Context::Instance());
}

TEST_F(ContextTest, SetLogger) {
  LoggerRef logger(new NoopLogger());
  this->context->initialise(logger);
  ASSERT_EQ(logger, Context::Logger());
}

TEST_F(ContextTest, SetSourceManager) {
  LoopManagerRef manager(new NoopLoopManager());
  this->context->initialise(manager);
  ASSERT_EQ(manager, Context::LoopManager());
}
