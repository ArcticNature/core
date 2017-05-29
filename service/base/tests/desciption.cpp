// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <json.hpp>

#include "core/exceptions/configuration.h"
#include "core/exceptions/options.h"
#include "core/service/description.h"


using nlohmann::json;

using sf::core::exception::InvalidOption;
using sf::core::exception::MissingConfiguration;

using sf::core::service::ServiceDescription;
using sf::core::service::ServiceDescriptionBuilder;


class ServiceDescriptionBuilderTest : public ::testing::Test {
 protected:
  ServiceDescriptionBuilder builder() {
    ServiceDescriptionBuilder builder;
    builder.serviceId("abc");
    builder.configVersion("def");
    builder.connector("test");
    return builder;
  }
};


TEST_F(ServiceDescriptionBuilderTest, RequiresServiceID) {
  auto builder = ServiceDescription::Build();
  ASSERT_THROW(builder.build(), MissingConfiguration);
}

TEST_F(ServiceDescriptionBuilderTest, RequireConfigVersion) {
  auto builder = ServiceDescription::Build();
  builder.serviceId("abc");
  ASSERT_THROW(builder.build(), MissingConfiguration);
}

TEST_F(ServiceDescriptionBuilderTest, RequireConnector) {
  auto builder = ServiceDescription::Build();
  builder.serviceId("abc");
  builder.configVersion("def");
  ASSERT_THROW(builder.build(), MissingConfiguration);
}

TEST_F(ServiceDescriptionBuilderTest, SetsRequiredAttributes) {
  auto builder = this->builder();
  auto service = builder.build();
  ASSERT_EQ("abc", service.serviceId());
  ASSERT_EQ("def", service.configVersion());
  ASSERT_EQ("test", service.connector());
}

TEST_F(ServiceDescriptionBuilderTest, SetConnectorOptions) {
  auto builder = this->builder();
  builder.connectorConfig(json::array());
  auto service = builder.build();
  ASSERT_EQ(json::array(), service.connectorConfig());
}

TEST_F(ServiceDescriptionBuilderTest, SetEnvironment) {
  auto builder = this->builder();
  builder.environment("key1", "a");
  builder.environment("key2", "b");
  builder.environment("key1", "c");

  std::map<std::string, std::string> env = {
    {"key1", "c"},
    {"key2", "b"}
  };
  auto service = builder.build();
  ASSERT_EQ(env, service.environment());
}

TEST_F(ServiceDescriptionBuilderTest, SetPort) {
  auto builder = this->builder();
  builder.port("key1", 8010);
  builder.port("key2", 8020);
  builder.port("key1", 8030);

  std::map<std::string, uint32_t> ports = {
    {"key1", 8030},
    {"key2", 8020}
  };
  auto service = builder.build();
  ASSERT_EQ(ports, service.ports());
}

TEST_F(ServiceDescriptionBuilderTest, SetPortChecksValidity) {
  auto builder = this->builder();
  builder.port("key1", 8010);
  ASSERT_THROW(builder.port("key2", 8010), InvalidOption);
  ASSERT_THROW(builder.port("key3", 0), InvalidOption);
  ASSERT_THROW(builder.port("key4", 900000), InvalidOption);
}

TEST_F(ServiceDescriptionBuilderTest, SetGroup) {
  auto builder = this->builder();
  builder.group("group");
  auto service = builder.build();
  ASSERT_EQ("group", service.group());
}

TEST_F(ServiceDescriptionBuilderTest, SetUser) {
  auto builder = this->builder();
  builder.user("usr");
  auto service = builder.build();
  ASSERT_EQ("usr", service.user());
}


TEST(ServiceDescription, Load) {
  json jsonified = {
    {"__type", "ServiceDescription"},
    {"__version", 1},
    {"service_id", "abc"},
    {"config_version", "def"},

    {"connector", {
      {"name", "test"},
      {"config", json::array()}
    }},

    {"environment", {
      {"key1", "value1"},
      {"key2", "value2"}
    }},

    {"ports", {
      {"http", 80},
      {"ssh", 22}
    }},

    {"group", "me"},
    {"user", "me"}
  };

  std::map<std::string, std::string> env = {
    {"key1", "value1"},
    {"key2", "value2"}
  };
  std::map<std::string, uint32_t> ports = {
    {"http", 80},
    {"ssh", 22}
  };

  auto service = ServiceDescription::Load(jsonified);
  ASSERT_EQ("abc", service.serviceId());
  ASSERT_EQ("def", service.configVersion());
  ASSERT_EQ("test", service.connector());
  ASSERT_EQ(json::array(), service.connectorConfig()); 
  ASSERT_EQ(env, service.environment());
  ASSERT_EQ(ports, service.ports());
  ASSERT_EQ("me", service.group());
  ASSERT_EQ("me", service.user());
}

TEST(ServiceDescription, Jsonify) {
  auto builder = ServiceDescription::Build();
  builder.serviceId("abc");
  builder.configVersion("def");
  builder.connector("test");
  builder.connectorConfig(json::array());
  builder.environment("key1", "value1");
  builder.environment("key2", "value2");
  builder.port("http", 80);
  builder.port("ssh", 22);
  builder.group("me");
  builder.user("me");

  json expected = {
    {"__type", "ServiceDescription"},
    {"__version", 1},
    {"service_id", "abc"},
    {"config_version", "def"},

    {"connector", {
      {"name", "test"},
      {"config", json::array()}
    }},

    {"environment", {
      {"key1", "value1"},
      {"key2", "value2"}
    }},

    {"ports", {
      {"http", 80},
      {"ssh", 22}
    }},

    {"group", "me"},
    {"user", "me"}
  };
  auto service = builder.build();
  auto actual = service.jsonify();
  ASSERT_EQ(expected.dump(), actual.dump());
}

TEST(ServiceDescription, Hash) {
  auto builder = ServiceDescription::Build().connector("test");
  auto service1 = builder.serviceId("abc").configVersion("def").build();
  auto service2 = builder.serviceId("ghi").configVersion("jkl").build();

  std::size_t hash1 = 11681000328127197227U;
  std::size_t hash2 = 13280232137901175256U;
  ASSERT_EQ(hash1, service1.hash());
  ASSERT_EQ(hash1, service1.hash());
  ASSERT_EQ(hash2, service2.hash());
}
