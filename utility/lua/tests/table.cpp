// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <memory>
#include <gtest/gtest.h>

#include "core/utility/lua.h"
#include "core/utility/lua/table.h"

using sf::core::utility::Lua;
using sf::core::utility::LuaTable;


class LuaT : public Lua {
 public:
  lua_State* getState() {
    return this->state.get();
  }
};


class LuaTableT : public LuaTable {
 public:
  LuaTableT(Lua* state, int idx) : LuaTable(state, idx) {}
  lua_Integer getRef() {
    return this->table_ref;
  }
};


TEST(LuaTableRef, IsCreatedInConstructor) {
  LuaT lua;
  lua_State* state = lua.getState();

  // Create and wrap new table.
  lua_newtable(state);
  LuaTableT table(&lua, -1);

  // Push table again from reference.
  lua_Integer table_ref = table.getRef();
  lua_rawgeti(state, LUA_REGISTRYINDEX, table_ref);
  ASSERT_EQ(1, lua_compare(state, -1, -2, LUA_OPEQ));
}

TEST(LuaTableRef, IsRemovedByDestrutor) {
  LuaT lua;
  lua_State*  state = lua.getState();
  lua_Integer table_ref = 0;

  {
    // Create and wrap new table.
    lua_newtable(state);
    LuaTableT table(&lua, -1);
    table_ref = table.getRef();
  }

  lua_rawgeti(state, LUA_REGISTRYINDEX, table_ref);
  ASSERT_EQ(1, lua_isnil(state, -1));
}

TEST(LuaTableRef, StackManipulation) {
  LuaT lua;
  lua_State* state = lua.getState();

  // Wrap a new table and pop it from the stack.
  lua_newtable(state);
  LuaTable table1(&lua, -1, true);
  ASSERT_EQ(0, lua.stack()->size());

  // Wrap a new table but DO NOT pop it from the stack.
  lua_newtable(state);
  LuaTable table2(&lua, -1);
  ASSERT_EQ(1, lua.stack()->size());
}


TEST(LuaTableAccess, FromStack) {
  LuaT lua;
  lua_State* state = lua.getState();

  // Create the table.
  LuaTable table = lua.stack()->newTable();
  ASSERT_EQ(1, lua.stack()->size());

  // table["a"] = "abc";
  lua.stack()->push("abc");
  table.fromStack("a");
  ASSERT_EQ(1, lua.stack()->size());

  lua.stack()->push("a");
  lua_gettable(state, -2);
  ASSERT_EQ("abc", lua.stack()->toString(-1, true));
  ASSERT_EQ(1, lua.stack()->size());

  // table[2] = "def";
  lua.stack()->push("def");
  table.fromStack(2);
  ASSERT_EQ(1, lua.stack()->size());

  lua.stack()->push(2);
  lua_gettable(state, -2);
  ASSERT_EQ("def", lua.stack()->toString(-1, true));
  ASSERT_EQ(1, lua.stack()->size());
}

TEST(LuaTableAccess, ToStack) {
  LuaT lua;
  lua_State* state = lua.getState();

  // Create the table.
  LuaTable table = lua.stack()->newTable();
  ASSERT_EQ(1, lua.stack()->size());

  // table["a"] = "abc";
  lua.stack()->push("a");
  lua.stack()->push("abc");
  lua_settable(state, -3);
  ASSERT_EQ(1, lua.stack()->size());

  table.toStack("a");
  ASSERT_EQ("abc", lua.stack()->toString(-1, true));
  ASSERT_EQ(1, lua.stack()->size());

  // table[2] = "def";
  lua.stack()->push(2);
  lua.stack()->push("def");
  lua_settable(state, -3);
  ASSERT_EQ(1, lua.stack()->size());

  table.toStack(2);
  ASSERT_EQ("def", lua.stack()->toString(-1, true));
  ASSERT_EQ(1, lua.stack()->size());
}

TEST(LuaTableAccess, Int) {
  LuaT lua;
  lua_State* state = lua.getState();
  LuaTable   table = lua.stack()->newTable();
  ASSERT_EQ(1, lua.stack()->size());

  // Store value.
  table.set("a", 3);
  table.set(2, 6);

  // Check it was stored.
  lua.stack()->push("a");
  lua_gettable(state, -2);
  ASSERT_EQ(3, lua.stack()->toInt(-1, true));

  lua.stack()->push(2);
  lua_gettable(state, -2);
  ASSERT_EQ(6, lua.stack()->toInt(-1, true));

  // Ensure stack integirty.
  ASSERT_EQ(1, lua.stack()->size());

  // Read value.
  ASSERT_EQ(3, table.toInt("a"));
  ASSERT_EQ(6, table.toInt(2));
  ASSERT_EQ(1, lua.stack()->size());
}

TEST(LuaTableAccess, String) {
  LuaT lua;
  lua_State* state = lua.getState();
  LuaTable   table = lua.stack()->newTable();
  ASSERT_EQ(1, lua.stack()->size());

  // Store value.
  table.set("a", "abc");
  table.set(2, "def");

  // Check it was stored.
  lua.stack()->push("a");
  lua_gettable(state, -2);
  ASSERT_EQ("abc", lua.stack()->toString(-1, true));

  lua.stack()->push(2);
  lua_gettable(state, -2);
  ASSERT_EQ("def", lua.stack()->toString(-1, true));

  // Ensure stack integirty.
  ASSERT_EQ(1, lua.stack()->size());

  // Read value.
  ASSERT_EQ("abc", table.toString("a"));
  ASSERT_EQ("def", table.toString(2));
  ASSERT_EQ(1, lua.stack()->size());
}
