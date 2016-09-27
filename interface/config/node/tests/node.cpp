// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/context.h"
#include "core/cluster/node.h"

#include "core/exceptions/base.h"
#include "core/exceptions/configuration.h"

#include "core/model/repository.h"
#include "core/utility/lua.h"

#include "./base.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::cluster::Node;

using sf::core::exception::ContextUninitialised;
using sf::core::exception::InvalidConfiguration;

using sf::core::model::RepositoryVersionId;
using sf::core::utility::Lua;

using sf::core::test::NodeConfigLoaderTest;
using sf::core::test::NodeConfigIntentsOrderTest;


TEST_F(NodeConfigLoaderTest, InitLua) {
  this->loader->load();
  Lua* lua = this->loader->getLua();
  lua->doString("return core");
  ASSERT_EQ(LUA_TTABLE, lua->stack()->type());
  ASSERT_EQ(1, this->loader->init_count);
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
  ASSERT_THROW(context->sourceManager(), ContextUninitialised);
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
  Lua* lua = this->loader->getLua();
  // Null + EventManager
  ASSERT_EQ(2, this->loader->getIntents().size());
}

TEST_F(NodeConfigLoaderTest, CheckNewContext) {
  this->loader->load();
  ContextRef expected = this->loader->getContext();
  ContextRef actual = Context::instance();
  ASSERT_EQ(expected.get(), actual.get());
}

TEST_F(NodeConfigLoaderTest, CheckNewVersion) {
  this->loader->load();
  RepositoryVersionId version = Node::me()->configVersion();
  ASSERT_EQ("refs/heads/test-fixture", version.symbolic());
  ASSERT_EQ(this->loader->getEffective(), version.effective());
}


TEST_F(NodeConfigIntentsOrderTest, NullIntent) {
  this->loader->loadToSort();
  ASSERT_ORDER({
      "event.manager",
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
      "event.tcp",
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
      "event.tcp",
      "event.unix",
      "null"
  });
}

// event.tcp -> event.manager
TEST_F(NodeConfigIntentsOrderTest, MissingDepends) {
  this->useIntent("event_tcp");
  this->loader->loadToSort();
  ASSERT_ORDER({
      "event.manager",
      "event.tcp",
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
      "event.tcp.default",
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
      "event.tcp",
      "event.tcp.default",
      "null"
  });
}
