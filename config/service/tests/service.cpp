// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/config/hooks.h"
#include "core/config/service.h"
#include "core/exceptions/configuration.h"
#include "core/service/description.h"

#include "core/testing/hooks.h"
#include "core/testing/promise.h"


using ConfigLoaderHook = sf::core::hook::ConfigLoader;

using sf::core::config::ConfigLoader;
using sf::core::config::ServiceLoader;

using sf::core::exception::InvalidConfiguration;
using sf::core::exception::MissingConfiguration;
using sf::core::service::ServiceDescription;

using sf::core::testing::HookTest;


class ServiceLoaderTest : public HookTest {
 public:
  ServiceLoaderTest() {
    this->trackHook(&ConfigLoaderHook::InitialiseLua);
    this->clearHooks();
  }

  ConfigLoader forScript(std::string script) {
    return std::make_shared<ServiceLoader>(script, "<version>");
  }

  ~ServiceLoaderTest() {
    this->clearHooks();
  }
};


TEST_F(ServiceLoaderTest, NoCode) {
  auto on_fail = [](const std::exception_ptr& ex) {
    EXPECT_THROW(std::rethrow_exception(ex), MissingConfiguration);
    return nullptr;
  };
  auto on_success = []() {
    ADD_FAILURE() << "Expected load to fail but it completed.";
    return nullptr;
  };

  auto loader = this->forScript("");
  auto run = loader->load().then(on_success, on_fail);
  ASSERT_TRUE(run.settled());
}

TEST_F(ServiceLoaderTest, NotATable) {
  auto on_fail = [](const std::exception_ptr& ex) {
    EXPECT_THROW(std::rethrow_exception(ex), InvalidConfiguration);
    return nullptr;
  };
  auto on_success = []() {
    ADD_FAILURE() << "Expected load to fail but it completed.";
    return nullptr;
  };

  auto loader = this->forScript("__service = 123");
  auto run = loader->load().then(on_success, on_fail);
  ASSERT_TRUE(run.settled());
}

TEST_F(ServiceLoaderTest, ReturnsDescription) {
  auto loader = this->forScript(
      "__service = {\n"
      "  id = 'core.config.service',\n"
      "  conf = {\n"
      "    connector = {id='test'}\n"
      "  }\n"
      "}"
  );
  auto run = loader->load().then([](ServiceDescription service) {
    EXPECT_EQ("core.config.service", service.serviceId());
    return nullptr;
  });
  EXPECT_PROMISE_RESOLVED(run);
}

TEST_F(ServiceLoaderTest, DefinesServiceFunction) {
  auto loader = this->forScript(
      "service 'core.config.service' {\n"
      "  connector = {id='test'}\n"
      "}"
  );
  auto run = loader->load().then([](ServiceDescription service) {
    EXPECT_EQ("core.config.service", service.serviceId());
    return nullptr;
  });
  EXPECT_PROMISE_RESOLVED(run);
}

TEST_F(ServiceLoaderTest, DefinesServiceOnce) {
  auto on_fail = [](const std::exception_ptr& ex) {
    EXPECT_THROW(std::rethrow_exception(ex), InvalidConfiguration);
    return nullptr;
  };
  auto on_success = []() {
    ADD_FAILURE() << "Expected load to fail but it completed.";
    return nullptr;
  };

  auto loader = this->forScript(
      "service 'core.config.service1' {}\n"
      "service 'core.config.service2' {}"
  );
  auto run = loader->load().then(on_success, on_fail);
  ASSERT_TRUE(run.settled());
}
