// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/lua/registry.h"

#include <string>

#include "core/utility/lua.h"

using sf::core::utility::Lua;
using sf::core::utility::LuaRegistry;


LuaRegistry::LuaRegistry(Lua* state) {
  this->state = state;
}

void LuaRegistry::store(std::string key, void* lightUserData) {
  // Stack must have key and value at the top.
  lua_State* state = this->state->state.get();
  lua_pushstring(state, key.c_str());
  lua_pushlightuserdata(state, lightUserData);
  lua_rawset(state, LUA_REGISTRYINDEX);
}

void LuaRegistry::dereference(lua_Integer ref) {
  lua_State* state = this->state->state.get();
  lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
}

void LuaRegistry::freeReference(lua_Integer ref) {
  lua_State* state = this->state->state.get();
  luaL_unref(state, LUA_REGISTRYINDEX, ref);
}

lua_Integer LuaRegistry::referenceTop() {
  lua_State* state = this->state->state.get();
  return luaL_ref(state, LUA_REGISTRYINDEX);
}
