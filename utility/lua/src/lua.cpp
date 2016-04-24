// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/lua.h"

#include "core/exceptions/lua.h"
#include "core/utility/lua/registry.h"

#define LUA_SELF_REF_KEY "_sf_lua_self_ref"

using sf::core::exception::LuaGCError;
using sf::core::exception::LuaHandlerError;
using sf::core::exception::LuaInvalidState;
using sf::core::exception::LuaMemoryError;
using sf::core::exception::LuaRuntimeError;
using sf::core::exception::LuaSyntaxError;
using sf::core::exception::LuaTypeError;

using sf::core::utility::Lua;
using sf::core::utility::LuaRegistry;
using sf::core::utility::LuaStack;


Lua* Lua::fetchFrom(lua_State* state) {
  // Attempt to fetch Lua pointer.
  lua_pushstring(state, LUA_SELF_REF_KEY);
  lua_rawget(state, LUA_REGISTRYINDEX);
  if (!lua_islightuserdata(state, -1)) {
    throw LuaInvalidState();
  }

  // Fetch and cast the value on the stack.
  void* address = lua_touserdata(state, -1);
  lua_pop(state, 1);
  return static_cast<Lua*>(address);
}


void Lua::checkError(int code, std::string source_name) {
  switch(code) {
    case LUA_OK:        return;
    case LUA_ERRERR:    throw LuaHandlerError();
    case LUA_ERRGCMM:   throw LuaGCError();
    case LUA_ERRMEM:    throw LuaMemoryError();
    case LUA_ERRSYNTAX: throw LuaSyntaxError(source_name);
    case LUA_ERRRUN:
      throw LuaRuntimeError(this->stack()->toString(-1, true));
    default:
      throw LuaRuntimeError("Invalid state code.");
  }
}


Lua::Lua() : state(luaL_newstate(), lua_close) {
  // Initialise state and registry.
  this->_registry = std::shared_ptr<LuaRegistry>(new LuaRegistry(this));
  this->_stack = std::shared_ptr<LuaStack>(new LuaStack(this));

  // Store a reference to myself in the registry.
  this->_registry->store(LUA_SELF_REF_KEY, this);
}

Lua::~Lua() {
  // NOOP.
}

void Lua::doString(std::string code, std::string name) {
  int result = 0;
  lua_State* state = this->state.get();

  result = luaL_loadbuffer(state, code.c_str(), code.length(), name.c_str());
  this->checkError(result, name);

  result = lua_pcall(state, 0, LUA_MULTRET, 0);
  this->checkError(result, name);
}

LuaRegistry* Lua::registry() {
  return this->_registry.get();
}

LuaStack* Lua::stack() {
  return this->_stack.get();
}


LuaStack::LuaStack(Lua* state) {
  this->state = state;
}

void LuaStack::push(std::string value) {
  lua_State* state = this->state->state.get();
  lua_checkstack(state, 1);
  lua_pushstring(state, value.c_str());
}

int LuaStack::size() {
  return lua_gettop(this->state->state.get());
}

std::string LuaStack::toString(int index, bool pop) {
  lua_State* state = this->state->state.get();
  if (lua_type(state, index) != LUA_TSTRING) {
    throw LuaTypeError(
        "string",
        lua_typename(state, lua_type(state, index))
    );
  }

  std::string value = lua_tostring(state, index);
  if (pop) {
    lua_remove(state, index);
  }
  return value;
}
