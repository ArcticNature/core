// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/lua/table.h"

#include <string>

#include "core/utility/lua.h"
#include "core/utility/lua/registry.h"


using sf::core::utility::Lua;
using sf::core::utility::LuaTable;


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
  return LuaTable(this->state, -1, true);
}
