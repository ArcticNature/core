// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "lua.hpp"

#include "core/exceptions/lua.h"
#include "core/utility/lua.h"
#include "core/utility/lua/proxy.h"

using sf::core::exception::LuaRuntimeError;
using sf::core::exception::LuaTypeExists;

using sf::core::utility::Lua;
using sf::core::utility::LuaTypeProxy;
using sf::core::utility::lua_proxy_delete;


class LuaT : public Lua {
 public:
  lua_State* getState() {
    return this->state.get();
  }
};



class TestTypeProxy : public LuaTypeProxy {
 protected:
  static const struct luaL_Reg lib[];
  const luaL_Reg* library() const;

  static void gc_method(void* instance) {
    int* value = reinterpret_cast<int*>(instance);
    delete value;
  };

  TestTypeProxy(lua_State* state) : LuaTypeProxy(state) {}

  std::string typeId() const {
    return "ltp::test";
  }

  lua_proxy_delete deleter() const {
    return TestTypeProxy::gc_method;
  }

 public:
  static int test(lua_State* state) {
    return 0;
  }

  static int get_value(lua_State* state) {
    Lua* lua = Lua::fetchFrom(state);
    TestTypeProxy type(state);

    int* value = type.check<int>();
    lua->stack()->push(*value);
    return 1;
  }

  TestTypeProxy() : LuaTypeProxy() {}
};

const struct luaL_Reg TestTypeProxy::lib[] = {
  {"test",    TestTypeProxy::test},
  {"get_val", TestTypeProxy::get_value},
  {nullptr,   nullptr}
};

const luaL_Reg* TestTypeProxy::library() const {
  return TestTypeProxy::lib;
}


TEST(LuaTypeProxy, InitialisedOnce) {
  Lua lua;
  TestTypeProxy test_type_proxy;
  test_type_proxy.initType(lua);
  ASSERT_THROW(test_type_proxy.initType(lua), LuaTypeExists);
}

TEST(LuaTypeProxy, RegistersFunctions) {
  LuaT lua;
  lua_State* state = lua.getState();
  TestTypeProxy test_type_proxy;

  // Create type.
  test_type_proxy.initType(lua);
  ASSERT_EQ(0, lua.stack()->size());
  luaL_getmetatable(state, "ltp::test");

  // Make sure the metatable does not have the methods.
  lua_pushstring(state, "test");
  lua_gettable(state, -2);
  ASSERT_EQ(LUA_TNIL, lua_type(state, -1));
  lua.stack()->remove(-1);

  // Make sure the __index table exists.
  lua_pushstring(state, "__index");
  lua_gettable(state, -2);
  ASSERT_EQ(LUA_TTABLE, lua_type(state, -1));

  // And check that the method exists.
  lua_pushstring(state, "test");
  lua_gettable(state, -2);
  ASSERT_EQ(LUA_TFUNCTION, lua_type(state, -1));
}

TEST(LuaTypeProxy, RegistersMetadata) {
  LuaT lua;
  lua_State* state = lua.getState();
  TestTypeProxy test_type_proxy;

  // Create type.
  test_type_proxy.initType(lua);
  ASSERT_EQ(0, lua.stack()->size());
  luaL_getmetatable(state, "ltp::test");

  // Make sure the __gc function exists.
  lua_pushstring(state, "__gc");
  lua_gettable(state, -2);
  ASSERT_EQ(LUA_TFUNCTION, lua_type(state, -1));
}

TEST(LuaTypeProxy, CreateType) {
  Lua lua;
  TestTypeProxy test_type_proxy;

  // Create type and instance.
  test_type_proxy.initType(lua);
  test_type_proxy.bind(lua, new int(5));
  ASSERT_EQ(1, lua.stack()->size());
}

TEST(LuaTypeProxy, InvalidMethodArgument) {
  Lua lua;
  TestTypeProxy test_type_proxy;

  // Create type and instance.
  test_type_proxy.initType(lua);
  test_type_proxy.bind(lua, new int(5));
  ASSERT_EQ(1, lua.stack()->size());

  // Save to global.
  lua.globals()->fromStack("num");
  ASSERT_EQ(0, lua.stack()->size());

  // Call the get_value method.
  ASSERT_THROW(lua.doString("return num.get_val(3)"), LuaRuntimeError);
}

TEST(LuaTypeProxy, MethodCall) {
  Lua lua;
  TestTypeProxy test_type_proxy;

  // Create type and instance.
  test_type_proxy.initType(lua);
  test_type_proxy.bind(lua, new int(5));
  ASSERT_EQ(1, lua.stack()->size());

  // Save to global.
  lua.globals()->fromStack("num");
  ASSERT_EQ(0, lua.stack()->size());

  // Call the get_value method.
  lua.doString("return num:get_val()");
  ASSERT_EQ(5, lua.stack()->toInt(-1, true));
}
