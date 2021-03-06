// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/cluster/cluster.h"
#include "core/context/context.h"

#include "core/exceptions/base.h"
#include "core/exceptions/configuration.h"

#include "core/model/repository.h"
#include "core/utility/lua.h"

#include "./base.h"


using sf::core::cluster::Cluster;
using sf::core::context::Context;
using sf::core::context::ContextRef;

using sf::core::exception::ContextUninitialised;
using sf::core::exception::InvalidConfiguration;

using sf::core::model::RepositoryVersionId;
using sf::core::utility::Lua;

using sf::core::test::NodeConfigLoaderTest;
using sf::core::test::NodeConfigIntentsOrderTest;


TEST_F(NodeConfigLoaderTest, InitLua) {
  Lua* lua = this->loader->getLua();
  this->loader->load();
  ASSERT_EQ(1, this->loader->init_count);

  lua->doString("return core");
  ASSERT_EQ(LUA_TTABLE, lua->stack()->type());

  lua->doString("return cluster");
  ASSERT_EQ(LUA_TTABLE, lua->stack()->type());
}

TEST_F(NodeConfigLoaderTest, InitLuaOnce) {
  this->loader->load();
  this->loader->load();
  ASSERT_EQ(1, this->loader->init_count);
}

TEST_F(NodeConfigLoaderTest, NewContext) {
  this->loader->load();
  ContextRef context = this->loader->getContext();
  ASSERT_NE(nullptr, context.get());
  ASSERT_THROW(context->loopManager(), ContextUninitialised);
}

TEST_F(NodeConfigLoaderTest, ProcessesFiles) {
  this->loader->load();
  Lua* lua = this->loader->getLua();
  lua->doString("return core.test");
  ASSERT_EQ(LUA_TSTRING, lua->stack()->type());
  ASSERT_EQ("Test", lua->stack()->toString());
}

TEST_F(NodeConfigLoaderTest, IntentsAreCollected) {
  this->loader->load();
  // Null + EventManager + CoreMeta + ClusterMeta
  ASSERT_EQ(4, this->loader->getIntents().size());
}

TEST_F(NodeConfigLoaderTest, CheckNewContext) {
  this->loader->load();
  ContextRef expected = this->loader->getContext();
  ContextRef actual = Context::Instance();
  ASSERT_EQ(expected.get(), actual.get());
}

TEST_F(NodeConfigLoaderTest, CheckNewVersion) {
  this->loader->load();
  RepositoryVersionId version = Cluster::Instance()->myself()->configVersion();
  ASSERT_EQ("refs/heads/test-fixture", version.symbolic());
  ASSERT_EQ(this->loader->getEffective(), version.effective());
}


TEST_F(NodeConfigIntentsOrderTest, NullIntent) {
  this->loader->loadToSort();
  ASSERT_ORDER({
      "event.manager",
      "cluster.metadata",
      "core.metadata",
      "null"
  });
}

// event.tcp -> event.manager
TEST_F(NodeConfigIntentsOrderTest, SimpleDepends) {
  this->useIntent("event_tcp");
  this->useIntent("event_manager");
  this->loader->loadToSort();
  ASSERT_ORDER({
      "event.manager",
      "cluster.metadata",
      "event.tcp",
      "core.metadata",
      "null"
  });
}

// cycle1 <-> cycle2
TEST_F(NodeConfigIntentsOrderTest, CircularDepends) {
  this->useIntent("cycle1");
  this->useIntent("cycle2");
  ASSERT_THROW(this->loader->loadToSort(), InvalidConfiguration);
}

// event.tcp  \
//             => event.manager
// event.unix /
TEST_F(NodeConfigIntentsOrderTest, TcpUnixDepends) {
  this->useIntent("event_tcp");
  this->useIntent("event_manager");
  this->useIntent("event_unix");
  this->loader->loadToSort();
  ASSERT_ORDER({
      "event.manager",
      "cluster.metadata",
      "event.tcp",
      "event.unix",
      "core.metadata",
      "null"
  });
}

// event.tcp -> event.manager
TEST_F(NodeConfigIntentsOrderTest, MissingDepends) {
  this->useIntent("event_tcp");
  this->loader->loadToSort();
  ASSERT_ORDER({
      "event.manager",
      "cluster.metadata",
      "event.tcp",
      "core.metadata",
      "null"
  });
}

// event.tcp.default -?> event.tcp
TEST_F(NodeConfigIntentsOrderTest, OptionalMissing) {
  this->useIntent("event_tcp_default");
  this->useIntent("event_manager");
  this->loader->loadToSort();
  ASSERT_ORDER({
      "event.manager",
      "cluster.metadata",
      "event.tcp.default",
      "core.metadata",
      "null"
  });
}

TEST_F(NodeConfigIntentsOrderTest, OptionalSorted) {
  this->useIntent("event_tcp_default");
  this->useIntent("event_tcp");
  this->useIntent("event_manager");
  this->loader->loadToSort();
  ASSERT_ORDER({
      "event.manager",
      "cluster.metadata",
      "event.tcp",
      "event.tcp.default",
      "core.metadata",
      "null"
  });
}
