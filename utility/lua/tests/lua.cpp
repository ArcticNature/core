// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <memory>
#include <gtest/gtest.h>

#include "core/exceptions/lua.h"
#include "core/utility/lua.h"

using sf::core::exception::LuaInvalidState;
using sf::core::exception::LuaRuntimeError;
using sf::core::exception::LuaSyntaxError;
using sf::core::exception::LuaTypeError;
using sf::core::utility::Lua;


class LuaT : public Lua {
 public:
  lua_State* getState() {
    return this->state.get();
  }
};


TEST(LuaFetchFromState, Error) {
  std::shared_ptr<lua_State> state(luaL_newstate(), lua_close);
  ASSERT_THROW(Lua::fetchFrom(state.get()), LuaInvalidState);
}

TEST(LuaFetchFromState, Success) {
  LuaT lua;
  lua_State* state = lua.getState();
  ASSERT_EQ(&lua, Lua::fetchFrom(state));
}


TEST(LuaRunString, Pass) {
  Lua lua;
  lua.doString("return 2 + 2");
}

TEST(LuaRunString, SyntaxError) {
  Lua lua;
  ASSERT_THROW(lua.doString("2a _ test"), LuaSyntaxError);
}

TEST(LuaRunString, RuntimeError) {
  Lua lua;
  ASSERT_THROW(lua.doString("return f()"), LuaRuntimeError);
}


TEST(LuaGlobals, API) {
  Lua lua;
  lua.globals()->set("two", 2);
  ASSERT_EQ(2, lua.globals()->toInt("two"));

  lua.doString("return two + 2");
  ASSERT_EQ(4, lua.stack()->toInt(-1, true));
}


TEST(LuaStack, AbsoluteIndex) {
  Lua lua;
  ASSERT_THROW(lua.stack()->absoluteIndex(0),  LuaInvalidState);
  ASSERT_THROW(lua.stack()->absoluteIndex(1),  LuaInvalidState);
  ASSERT_THROW(lua.stack()->absoluteIndex(-1), LuaInvalidState);

  lua.stack()->push("abc");
  lua.stack()->push("def");
  lua.stack()->push("ghk");
  ASSERT_EQ(3, lua.stack()->absoluteIndex(-1));
  ASSERT_EQ(2, lua.stack()->absoluteIndex(-2));
  ASSERT_EQ(1, lua.stack()->absoluteIndex(-3));
  ASSERT_THROW(lua.stack()->absoluteIndex(-4), LuaInvalidState);

  ASSERT_EQ(LUA_REGISTRYINDEX, lua.stack()->absoluteIndex(LUA_REGISTRYINDEX));
}

TEST(LuaStack, Size) {
  Lua lua;
  ASSERT_EQ(0, lua.stack()->size());

  lua.doString("return 'abc'");
  ASSERT_EQ(1, lua.stack()->size());

  lua.doString("return 2 + 2");
  ASSERT_EQ(2, lua.stack()->size());
}

TEST(LuaStack, Int) {
  Lua lua;
  lua.doString("return 2");

  // Peek.
  ASSERT_EQ(2, lua.stack()->toInt());
  ASSERT_EQ(2, lua.stack()->toInt());

  // Pop.
  ASSERT_EQ(2, lua.stack()->toInt(-1, true));
  ASSERT_EQ(0, lua.stack()->size());

  // Push.
  lua.stack()->push(2);
  ASSERT_EQ(2, lua.stack()->toInt());

  // Wrong type.
  lua.doString("return 'abc'");
  ASSERT_THROW(lua.stack()->toInt(), LuaTypeError);
}

TEST(LuaStack, String) {
  Lua lua;
  lua.doString("return 'abc'");

  // Peek.
  ASSERT_EQ("abc", lua.stack()->toString());
  ASSERT_EQ("abc", lua.stack()->toString());

  // Pop.
  ASSERT_EQ("abc", lua.stack()->toString(-1, true));
  ASSERT_EQ(0, lua.stack()->size());

  // Push.
  lua.stack()->push("def");
  ASSERT_EQ("def", lua.stack()->toString());

  // Wrong type.
  lua.doString("return 2");
  ASSERT_THROW(lua.stack()->toString(), LuaTypeError);
}
