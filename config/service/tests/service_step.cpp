// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/config/base.h"
#include "core/config/hooks.h"

#include "core/config/service.h"
#include "core/config/detail/service_step.h"
#include "core/service/description.h"

#include "core/testing/hooks.h"
#include "core/testing/promise.h"


using nlohmann::json;
using poolqueue::Promise;
using ConfigLoaderHook = sf::core::hook::ConfigLoader;

using sf::core::config::ConfigLoader;
using sf::core::config::ConfigStep_;

using sf::core::config::ServiceDescriptionStep;
using sf::core::config::ServiceLoader;
using sf::core::service::ServiceDescription;

using sf::core::utility::Lua;
using sf::core::utility::LuaTable;

using sf::core::testing::HookTest;


class TestableServiceStep_ : public ServiceDescriptionStep {
 public:
  using ServiceDescriptionStep::ServiceDescriptionStep;

  Lua lua() {
    return ConfigStep_::lua();
  }
};
typedef std::shared_ptr<TestableServiceStep_> TestableServiceStep;


class ServiceStepTest : public HookTest {
 public:
  ServiceStepTest() {
    this->trackHook(&ConfigLoaderHook::InitialiseLua);
    this->clearHooks();
    this->loader_ = std::make_shared<ServiceLoader>("", "abcd");
    this->step_ = std::make_shared<TestableServiceStep_>(
        this->loader_, "abcd"
    );
    this->lua_ = this->step_->lua();
  }

  ~ServiceStepTest() {
    this->loader_.reset();
    this->clearHooks();
  }

  LuaTable makeService(std::string id) {
    LuaTable service = this->lua_.globals()->newTable("__service");
    service.newTable("conf");
    service.set("id", id);
    return service;
  }

  Promise execute() {
    return this->step_->verify().then([this]() {
      return this->step_->execute();
    });
  }

 protected:
  ConfigLoader loader_;
  TestableServiceStep step_;
  Lua lua_;
};


TEST_F(ServiceStepTest, SetProperties) {
  // Result consts.
  std::map<std::string, std::string> expect_env = {
    {"DB_HOST", "localhost"},
    {"OTHER_VAR", "set"}
  };
  std::map<std::string, uint32_t> expect_ports = {
    {"http", 8080},
    {"ssh",  2222}
  };
  json expect_connector_conf = {
    {"cmd", "test!"}
  };

  // Setup.
  auto service = this->makeService("core.config.service");
  auto conf = service.toTable("conf");
  auto connector = conf.newTable("connector");
  auto env = conf.newTable("environment");
  auto ports = conf.newTable("ports");
  conf.set("group", "group");
  conf.set("user", "user");
  connector.set("id", "test");
  connector.set("cmd", "test!");
  env.set("DB_HOST", "localhost");
  env.set("OTHER_VAR", "set");
  ports.set("http", 8080);
  ports.set("ssh", 2222);

  // Run and assert.
  auto run = this->execute().then([&](ServiceDescription desc) {
    EXPECT_EQ("core.config.service", desc.serviceId());
    EXPECT_EQ("abcd", desc.configVersion());
    EXPECT_EQ("test", desc.connector());
    EXPECT_EQ(8080, desc.port("http"));
    EXPECT_EQ(expect_ports, desc.ports());
    EXPECT_EQ(
        expect_connector_conf.dump(),
        desc.connectorConfig().dump()
    );
    EXPECT_EQ(expect_env, desc.environment());
    EXPECT_EQ("group", desc.group());
    EXPECT_EQ("user", desc.user());
    return desc;
  });
  EXPECT_PROMISE_NO_THROW(run);
}
