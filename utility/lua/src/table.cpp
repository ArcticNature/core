// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/lua/table.h"

#include <json.hpp>
#include <string>

#include "core/exceptions/base.h"
#include "core/exceptions/lua.h"

#include "core/utility/lua.h"
#include "core/utility/lua/registry.h"


using nlohmann::json;

using sf::core::exception::LuaTypeError;
using sf::core::exception::NotImplemented;
using sf::core::exception::TypeError;

using sf::core::utility::Lua;
using sf::core::utility::LuaTable;


json LuaValueToJson(Lua* lua, int idx) {
  auto stack = lua->stack();
  auto type  = stack->type(idx);
  json value;

  switch (type) {
    case LUA_TBOOLEAN:
      value = stack->toBoolean(idx);
      return value;

    case LUA_TNUMBER:
      value = stack->toInt(idx);
      return value;

    case LUA_TNONE:
    case LUA_TNIL:
      return json();

    case LUA_TSTRING:
      value = stack->toString(idx);
      return value;

    case LUA_TTABLE:
      LuaTable table(lua, idx, false);
      bool table_has_int_keys = false;
      bool table_has_string_keys = false;
      value = json::object();

      // Assume this is a table (not an array).
      table.forEach([&](std::string key) {  // NOLINT(build/c++11)
        table_has_string_keys = true;
        table.toStack(key);
        value[key] = LuaValueToJson(lua, -1);
        stack->remove(-1);
      });

      // Check table is an array or has not int keys
      if (!table_has_string_keys) {
        value = json::array();
      }
      table.forEach([&](int key) {  // NOLINT(build/c++11)
        if (table_has_string_keys) {
          throw TypeError("Table with both int and string keys found");
        }
        table_has_int_keys = true;
        table.toStack(key);
        value.push_back(LuaValueToJson(lua, -1));
        stack->remove(-1);
      });

      // If the table hasno key return an empty object.
      if (!(table_has_int_keys || table_has_string_keys)) {
        return json::object();
      }
      return value;
  }

  // Unsupported type found.
  throw TypeError(
      "Table has a value with an usupported type: " +
      lua->typeName(type)
  );
}


LuaTable::LuaTable(Lua* state, int index, bool pop) {
  this->state = state;
  this->state->stack()->duplicate(index);
  this->table_ref = this->state->registry()->referenceTop();
  if (pop) {
    this->state->stack()->remove(index);
  }
}

LuaTable::~LuaTable() {
  this->state->registry()->freeReference(this->table_ref);
}

void LuaTable::forEach(std::function<void(int key)> fn) {
  auto state = this->state->state.get();
  auto stack = this->state->stack();
  int  top = stack->size() + 2;

  // Stack: [..., Table, Nil]
  this->pushMe();
  stack->pushNil();

  // Iterate over table until it is over.
  while (lua_next(state, -2) != 0) {
    // Stack: [..., Table, Key, Value] -> pop value.
    stack->remove(-1);

    // If key is not a number skip it.
    if (stack->type(-1) != LUA_TNUMBER) {
      continue;
    }

    // Fetch the key and call the functor.
    int key = stack->toInt(-1);
    fn(key);

    // Assert the stack is safe.
    int new_top = stack->size();
    if (new_top != top) {
      // TODO(stefano): LuaTable::forEach<int> stack check
      throw NotImplemented("LuaTable::forEach<int> stack check");
    }
  }

  // Pop left over key.
  stack->remove(-1);
}

void LuaTable::forEach(std::function<void(std::string key)> fn) {
  auto state = this->state->state.get();
  auto stack = this->state->stack();
  int  top = stack->size() + 2;

  // Stack: [..., Table, Nil]
  this->pushMe();
  stack->pushNil();

  // Iterate over table until it is over.
  while (lua_next(state, -2) != 0) {
    // Stack: [..., Table, Key, Value] -> pop value.
    stack->remove(-1);

    // If key is not a number skip it.
    if (stack->type(-1) == LUA_TNUMBER) {
      continue;
    }

    // Fetch the key and call the functor.
    std::string key = stack->toString(-1);
    fn(key);

    // Assert the stack is safe.
    int new_top = stack->size();
    if (new_top != top) {
      // TODO(stefano): LuaTable::forEach<string> stack check
      throw NotImplemented("LuaTable::forEach<string> stack check");
    }
  }

  // Pop left over key.
  stack->remove(-1);
}

void LuaTable::fromStack(std::string key, int index) {
  // lua_settable expects the stack to be table, [...], key, value.
  int abs_idx = this->state->stack()->absoluteIndex(index);
  this->state->stack()->check(3);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  this->state->stack()->duplicate(abs_idx);
  this->state->stack()->remove(abs_idx);
  lua_settable(this->state->state.get(), -3);
  this->state->stack()->remove(-1);
}

void LuaTable::fromStack(int key, int index) {
  // lua_settable expects the stack to be table, [...], key, value.
  int abs_idx = this->state->stack()->absoluteIndex(index);
  this->state->stack()->check(3);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  this->state->stack()->duplicate(abs_idx);
  this->state->stack()->remove(abs_idx);
  lua_settable(this->state->state.get(), -3);
  this->state->stack()->remove(-1);
}

bool LuaTable::has(int key) {
  this->toStack(key);
  auto stack = this->state->stack();
  int type = stack->type(-1);
  stack->remove(-1);
  return type != LUA_TNONE && type != LUA_TNIL;
}

bool LuaTable::has(std::string key) {
  this->toStack(key);
  auto stack = this->state->stack();
  int type = stack->type(-1);
  stack->remove(-1);
  return type != LUA_TNONE && type != LUA_TNIL;
}

LuaTable LuaTable::newTable(std::string key) {
  this->state->stack()->check(1);
  LuaTable table = this->state->stack()->newTable(false);
  this->fromStack(key);
  return table;
}

LuaTable LuaTable::newTable(int key) {
  this->state->stack()->check(1);
  LuaTable table = this->state->stack()->newTable(false);
  this->fromStack(key);
  return table;
}

void LuaTable::toStack(std::string key) {
  // lua_gettable expects the stack to be table, [...], key.
  this->state->stack()->check(2);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  lua_gettable(this->state->state.get(), -2);
  this->state->stack()->remove(-2);
}

void LuaTable::toStack(int key) {
  // lua_gettable expects the stack to be table, [...], key.
  this->state->stack()->check(2);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  lua_gettable(this->state->state.get(), -2);
  this->state->stack()->remove(-2);
}

void LuaTable::pushMe() {
  this->state->stack()->check(1);
  this->state->registry()->dereference(this->table_ref);
}

void LuaTable::remove(int key) {
  this->state->stack()->pushNil();
  this->fromStack(key);
}

void LuaTable::remove(std::string key) {
  this->state->stack()->pushNil();
  this->fromStack(key);
}

void LuaTable::set(int key, int value) {
  this->state->stack()->check(3);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  this->state->stack()->push(value);
  lua_settable(this->state->state.get(), -3);
  this->state->stack()->remove(-1);
}

void LuaTable::set(int key, std::string value) {
  this->state->stack()->check(3);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  this->state->stack()->push(value);
  lua_settable(this->state->state.get(), -3);
  this->state->stack()->remove(-1);
}

void LuaTable::set(std::string key, int value) {
  this->state->stack()->check(3);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  this->state->stack()->push(value);
  lua_settable(this->state->state.get(), -3);
  this->state->stack()->remove(-1);
}

void LuaTable::set(std::string key, std::string value) {
  this->state->stack()->check(3);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  this->state->stack()->push(value);
  lua_settable(this->state->state.get(), -3);
  this->state->stack()->remove(-1);
}

void LuaTable::set(std::string key, lua_CFunction value) {
  this->state->stack()->check(3);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  this->state->stack()->push(value, 0);
  lua_settable(this->state->state.get(), -3);
  this->state->stack()->remove(-1);
}

void LuaTable::setMetatable(int index) {
  int abs_idx = this->state->stack()->absoluteIndex(index);
  this->state->stack()->check(2);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->duplicate(abs_idx);
  this->state->stack()->remove(abs_idx);
  lua_setmetatable(this->state->state.get(), -2);
  this->state->stack()->remove(-1);
}

json LuaTable::toJSON() {
  this->pushMe();
  json value = LuaValueToJson(this->state, -1);
  this->state->stack()->remove(-1);
  return value;
}

int LuaTable::toInt(int key) {
  this->state->stack()->check(2);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  lua_gettable(this->state->state.get(), -2);
  this->state->stack()->remove(-2);
  return this->state->stack()->toInt(-1, true);
}

int LuaTable::toInt(std::string key) {
  this->state->stack()->check(2);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  lua_gettable(this->state->state.get(), -2);
  this->state->stack()->remove(-2);
  return this->state->stack()->toInt(-1, true);
}

std::string LuaTable::toString(int key) {
  this->state->stack()->check(2);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  lua_gettable(this->state->state.get(), -2);
  this->state->stack()->remove(-2);
  return this->state->stack()->toString(-1, true);
}

std::string LuaTable::toString(std::string key) {
  this->state->stack()->check(2);
  this->state->registry()->dereference(this->table_ref);
  this->state->stack()->push(key);
  lua_gettable(this->state->state.get(), -2);
  this->state->stack()->remove(-2);
  return this->state->stack()->toString(-1, true);
}

LuaTable LuaTable::toTable(std::string key) {
  this->toStack(key);
  auto type = this->state->stack()->type(-1);
  if (type != LUA_TTABLE) {
    throw LuaTypeError(
        "table",
        lua_typename(this->state->state.get(), type)
    );
  }
  return LuaTable(this->state, -1, true);
}
