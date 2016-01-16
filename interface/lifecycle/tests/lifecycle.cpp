// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/interface/lifecycle.h"

using sf::core::exception::AbortException;
using sf::core::exception::ImpossiblePath;
using sf::core::exception::StopException;

using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;

using sf::core::interface::Lifecycle;
using sf::core::interface::LifecycleHandler;
using sf::core::interface::LifecycleHandlerRef;
using sf::core::interface::LifecycleInstance;


class TestArgument : public BaseLifecycleArg {
 public:
  int count;

  TestArgument() {
    this->count = 0;
  }
};


class TestBaseHandler : public BaseLifecycleHandler {
 public:
  void handle(std::string event, BaseLifecycleArg* argument) {
    TestArgument* arg = dynamic_cast<TestArgument*>(argument);
    arg->count += 1;
  }
};


TEST(Lifecycle, NoHandlers) {
  LifecycleInstance instance;
  instance.trigger("test::event");
}

TEST(Lifecycle, EventHandled) {
  LifecycleInstance instance;
  TestArgument arg;
  LifecycleHandlerRef handler(new TestBaseHandler());

  instance.on("test::event", handler);
  instance.trigger("test::event", &arg);
  ASSERT_EQ(1, arg.count);
}

TEST(Lifecycle, EventHandledMultipleTimes) {
  LifecycleInstance instance;
  TestArgument arg;
  LifecycleHandlerRef handler(new TestBaseHandler());

  instance.on("test::event", handler);
  instance.on("test::event", handler);
  instance.trigger("test::event", &arg);
  ASSERT_EQ(2, arg.count);
}

TEST(Lifecycle, EventHandlerRemoved) {
  LifecycleInstance instance;
  TestArgument arg;
  LifecycleHandlerRef handler(new TestBaseHandler());

  instance.on("test::event", handler);
  instance.off("test::event", handler);
  instance.trigger("test::event", &arg);
  ASSERT_EQ(0, arg.count);
}


class TestLifecycleProxy : public ::testing::Test {
 public:
  ~TestLifecycleProxy() {
    Lifecycle::reset();
  }
};


TEST_F(TestLifecycleProxy, HandlesEvents) {
  TestArgument arg;
  LifecycleHandlerRef handler(new TestBaseHandler());

  Lifecycle::on("test::event", handler);
  Lifecycle::trigger("test::event", &arg);
  ASSERT_EQ(1, arg.count);
}

TEST_F(TestLifecycleProxy, EventHandlerRemoved) {
  TestArgument arg;
  LifecycleHandlerRef handler(new TestBaseHandler());

  Lifecycle::on("test::event", handler);
  Lifecycle::off("test::event", handler);
  Lifecycle::trigger("test::event", &arg);
  ASSERT_EQ(0, arg.count);
}


class TestArgumentType : public BaseLifecycleArg {};


class TestHandler : public LifecycleHandler<TestArgument> {
 public:
  bool called;

  TestHandler() {
    this->called = false;
  }

  void handle(std::string event, TestArgument* arg) {
    this->called = true;

    if (arg == nullptr) {
      return;
    }
    arg->count += 1;
  }
};


class TestTypeHandler : public LifecycleHandler<TestArgumentType> {
 public:
  bool called;

  TestTypeHandler() {
    this->called = false;
  }

  void handle(std::string event, TestArgumentType* arg) {
    this->called = true;
  }
};


TEST(LifecycleHandler, AllowsNullptr) {
  LifecycleInstance instance;
  TestHandler* h = new TestHandler();
  LifecycleHandlerRef handler(h);

  instance.on("test::event", handler);
  instance.trigger("test::event");
  ASSERT_TRUE(h->called);
}

TEST(LifecycleHandler, PassesClass) {
  LifecycleInstance instance;
  TestArgument arg;
  LifecycleHandlerRef handler(new TestHandler());

  instance.on("test::event", handler);
  instance.trigger("test::event", &arg);
  ASSERT_EQ(1, arg.count);
}

TEST(LifecycleHandler, SkipsHandlerOnTypeError) {
  TestTypeHandler* h2 = new TestTypeHandler();
  LifecycleInstance instance;
  TestArgument arg;

  LifecycleHandlerRef handler1(new TestHandler());
  LifecycleHandlerRef handler2(h2);

  instance.on("test::event", handler1);
  instance.on("test::event", handler2);
  instance.trigger("test::event", &arg);

  ASSERT_EQ(1, arg.count);
  ASSERT_FALSE(h2->called);
}


class TestAbortHandler : public LifecycleHandler<TestArgument> {
 public:
  void handle(std::string event, TestArgument* arg) {
    arg->count += 1;
    throw AbortException("m", 2, {"a", "b", "c"});
  }
};


class TestErrorHandler : public LifecycleHandler<TestArgument> {
 public:
  void handle(std::string event, TestArgument* arg) {
    arg->count += 1;
    throw ImpossiblePath();
  }
};


class TestStopHandler : public LifecycleHandler<TestArgument> {
 public:
  void handle(std::string event, TestArgument* arg) {
    arg->count += 1;
    throw StopException();
  }
};


TEST(LifecycleExceptions, StopException) {
  LifecycleInstance instance;
  TestArgument arg;
  LifecycleHandlerRef handler(new TestStopHandler());

  instance.on("test::event", handler);
  instance.on("test::event", handler);
  instance.on("test::event", handler);
  instance.trigger("test::event", &arg);
  ASSERT_EQ(1, arg.count);
}

TEST(LifecycleExceptions, AbortException) {
  LifecycleInstance instance;
  TestArgument arg;
  LifecycleHandlerRef handler(new TestAbortHandler());

  instance.on("test::event", handler);
  instance.on("test::event", handler);
  instance.on("test::event", handler);

  try {
    instance.trigger("test::event", &arg);
    ASSERT_FALSE(true);

  } catch (AbortException& ex) {
    ASSERT_EQ(2, ex.getCode());
    ASSERT_EQ("a\nb\nc", ex.getTrace());
    ASSERT_STREQ("m", ex.what());
  }

  ASSERT_EQ(1, arg.count);
}

TEST(LifecycleExceptions, IgnoreOtherExceptions) {
  LifecycleInstance instance;
  TestArgument arg;
  LifecycleHandlerRef handler(new TestErrorHandler());

  instance.on("test::event", handler);
  instance.on("test::event", handler);
  instance.on("test::event", handler);
  instance.trigger("test::event", &arg);
  ASSERT_EQ(3, arg.count);
}
