// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <exception>

#include "core/hook.h"

using sf::core::hook::Hook;
using sf::core::hook::ErrorHook;
using sf::core::hook::SignalHook;


class TestHook : public Hook<int, int> {
 public:
  int countCallbacks() {
    return this->callbacks_.size();
  }
};


class HookTest : public ::testing::Test {
 public:
  TestHook hook;
};


TEST_F(HookTest, Attach) {
  auto cb = [](int a, int b) { return; };
  this->hook.attach(cb);
  ASSERT_EQ(1, this->hook.countCallbacks());
}

TEST_F(HookTest, AttachTwice) {
  auto cb = [](int a, int b) { return; };
  this->hook.attach(cb);
  this->hook.attach(cb);
  ASSERT_EQ(2, this->hook.countCallbacks());
}

TEST_F(HookTest, Detach) {
  auto cb = [](int a, int b) { return; };
  this->hook.attach(cb);
  this->hook.detach(cb);
  ASSERT_EQ(0, this->hook.countCallbacks());
}

TEST_F(HookTest, DetachAll) {
  auto cb = [](int a, int b) { return; };
  this->hook.attach(cb);
  this->hook.attach(cb);
  this->hook.detach(cb);
  ASSERT_EQ(0, this->hook.countCallbacks());
}

TEST_F(HookTest, DetachOneOnly) {
  auto cb1 = [](int a, int b) { return; };
  auto cb2 = [](int a, int b) { return; };
  this->hook.attach(cb1);
  this->hook.attach(cb2);
  this->hook.detach(cb1);
  ASSERT_EQ(1, this->hook.countCallbacks());
}

TEST_F(HookTest, Trigger) {
  int total;
  auto cb = [&total](int a, int b) {
    total += a + b;
    return;
  };

  this->hook.attach(cb);
  this->hook.attach(cb);
  this->hook.trigger(1, 2);
  ASSERT_EQ(6, total);
}

TEST_F(HookTest, ExceptionsAreIgnored) {
  auto cb = [](int a, int b) {
    throw std::runtime_error("Test");
  };
  this->hook.attach(cb);
  ASSERT_NO_THROW(this->hook.trigger(1, 2));
}

TEST_F(HookTest, ExceptionsAreRethrown) {
  auto cb = [](int a, int b) {
    throw std::runtime_error("Test");
  };

  Hook<int, int> hook(true);
  hook.attach(cb);
  ASSERT_THROW(hook.trigger(1, 2), std::runtime_error);
}


TEST(ErrorHook, Trigger) {
  ErrorHook hook;
  std::string message;
  std::exception_ptr error = make_exception_ptr(std::runtime_error("test"));

  hook.attach([&message](std::exception_ptr error) {
      try {
        std::rethrow_exception(error);
      } catch (std::exception& ex) {
        message = ex.what();
      }
      return;
  });
  hook.trigger(error);
  ASSERT_EQ("test", message);
}

TEST(SignalHook, Trigger) {
  int call_count = 0;
  SignalHook hook;
  hook.attach([&call_count]() {
      call_count += 1;
      return;
  });
  hook.trigger();
  ASSERT_EQ(1, call_count);
}
