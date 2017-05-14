// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/lua.h"
#include "core/interface/config/node.h"
#include "core/utility/lua.h"

#include "./base.h"


using sf::core::exception::LuaRuntimeError;
using sf::core::interface::NodeConfigIntentLuaProxy;
using sf::core::utility::Lua;

using sf::core::test::MockIntent;
using sf::core::test::NodeConfigEventsFrom;


TEST_F(NodeConfigEventsFrom, FailWithNoArgs) {
  Lua* lua = this->loader->getLua();
  this->loader->initLua();
  ASSERT_THROW(lua->doString("core.events_from()"), LuaRuntimeError);
}

TEST_F(NodeConfigEventsFrom, Nil) {
  Lua* lua = this->loader->getLua();
  this->loader->initLua();
  lua->doString("return core.events_from(nil)");
  bool result = lua->stack()->toBoolean();
  ASSERT_FALSE(result);
}

TEST_F(NodeConfigEventsFrom, ExecFunction) {
  Lua* lua = this->loader->getLua();
  this->loader->initLua();
  lua->doString(
      "function test()\n"
      "  core._test = 30\n"
      "  return nil\n"
      "end"
  );
  lua->doString("return core.events_from(test)");
  bool result = lua->stack()->toBoolean();
  int _test = lua->globals()->toTable("core").toInt("_test");
  ASSERT_FALSE(result);
  ASSERT_EQ(30, _test);
}

TEST_F(NodeConfigEventsFrom, AddIntent) {
  NodeConfigIntentLuaProxy type;
  Lua* lua = this->loader->getLua();
  this->loader->initLua();
  lua->globals()->toTable("core").toStack("events_from");
  type.wrap(*lua, new MockIntent("test", "test"));
  lua->call(1, 1);
  bool result = lua->stack()->toBoolean();
  ASSERT_TRUE(result);
  ASSERT_EQ(2, this->loader->getMocks().size());
}
