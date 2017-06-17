// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <poolqueue/Promise.hpp>

#include "core/config/base.h"
#include "core/config/hooks.h"
#include "core/exceptions/base.h"
#include "core/utility/lua.h"

#include "core/testing/hooks.h"
#include "core/testing/promise.h"


using ConfigLoaderHook = sf::core::hook::ConfigLoader;

using poolqueue::Promise;

using sf::core::config::ConfigLoader;
using sf::core::config::ConfigLoader_;
using sf::core::config::ConfigStep;
using sf::core::config::ConfigStep_;

using sf::core::exception::SfException;
using sf::core::utility::Lua;
using sf::core::testing::HookTest;


class IntConfigStep : public ConfigStep_ {
 public:
  IntConfigStep(int config, ConfigLoader loader) : ConfigStep_(loader) {
    this->config_ = config;
  }

  Promise verify() {
    this->lua().globals()->set("verify", "yes");
    return Promise().settle();
  }

  Promise execute() {
    this->lua().globals()->set("execute", "yes");
    return Promise().settle(this->config_);
  }

 protected:
  int config_;
};


class TestBaseLoader : public ConfigLoader_ {
 public:
  Lua lua() {
    return this->lua_;
  }

  void initialise() {
    ConfigLoader_::initialise();
  }

  ConfigStep collectStep() {
    int config = this->lua_.globals()->toInt("config");
    return std::make_shared<IntConfigStep>(config, this->shared_from_this());
  }

  Promise runLua() {
    this->lua_.globals()->set("run_called", "yes");
    this->lua_.globals()->set("config", 55);
    return Promise().settle(this->shared_from_this());
  }

 protected:
  void initLuaEnv() {
    this->lua_.globals()->set("test", 42);
  }
};


class ConfigLoaderTest : public HookTest {
 public:
  std::shared_ptr<TestBaseLoader> loader;

  ConfigLoaderTest() {
    this->trackHook(&ConfigLoaderHook::InitialiseLua);
    this->clearHooks();
    this->loader = std::make_shared<TestBaseLoader>();
  }

  ~ConfigLoaderTest() {
    this->loader.reset();
    this->clearHooks();
  }
};


TEST_F(ConfigLoaderTest, Initialise) {
  Lua lua = this->loader->lua();
  ConfigLoaderHook::InitialiseLua.attach([](ConfigLoader loader, Lua lua) {
      lua.globals()->set("callback", "Called");
  });
  this->loader->initialiseLua.attach([](ConfigLoader loader, Lua lua) {
      lua.globals()->set("instance_callback", "Called");
  });

  lua.globals()->set("callback", "Not Called");
  lua.globals()->set("instance_callback", "Not Called");
  this->loader->initialise();
  ASSERT_EQ(42, lua.globals()->toInt("test"));
  ASSERT_EQ("Called", lua.globals()->toString("callback"));
  ASSERT_EQ("Called", lua.globals()->toString("instance_callback"));
}

TEST_F(ConfigLoaderTest, RunLua) {
  Lua lua = this->loader->lua();
  Promise run = this->loader->runLua();
  EXPECT_PROMISE_RESOLVED(run);
  ASSERT_EQ("yes", lua.globals()->toString("run_called"));
}

TEST_F(ConfigLoaderTest, CollectStepFails) {
  ASSERT_THROW(this->loader->collectStep(), SfException);
}

TEST_F(ConfigLoaderTest, CollectStepSuccess) {
  this->loader->lua().globals()->set("config", 33);
  auto step = this->loader->collectStep();
  ASSERT_NE(nullptr, step.get());

  auto verify = step->verify();
  EXPECT_PROMISE_RESOLVED(verify);

  auto execute = step->execute().then([](int value) {
      EXPECT_EQ(33, value);
      return nullptr;
  });
  EXPECT_PROMISE_RESOLVED(execute);
}

TEST_F(ConfigLoaderTest, Load) {
  auto loaded = this->loader->load().then([](int value) {
      EXPECT_EQ(55, value);
      return nullptr;
  });
  EXPECT_PROMISE_RESOLVED(loaded);

  auto lua = this->loader->lua();
  ASSERT_EQ("yes", lua.globals()->toString("verify"));
  ASSERT_EQ("yes", lua.globals()->toString("execute"));
}
