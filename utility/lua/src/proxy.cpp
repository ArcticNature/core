// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/lua/proxy.h"

#include <string>

#include "core/exceptions/lua.h"
#include "core/utility/lua.h"

using sf::core::exception::LuaTypeExists;
using sf::core::exception::LuaRuntimeError;
using sf::core::exception::LuaInvalidProxyUse;

using sf::core::utility::Lua;
using sf::core::utility::LuaTypeProxy;


int LuaTypeProxy::lua_proxy_gc(lua_State* state) {
  // Assume types are correct because of restricted access to this method.
  void* block   = lua_touserdata(state, 1);
  void* deleter = lua_touserdata(state, lua_upvalueindex(1));
  if (block == nullptr || deleter == nullptr) {
    return 0;
  }

  // Access the instance.
  void** pointer = reinterpret_cast<void**>(block);
  lua_proxy_delete fn = reinterpret_cast<lua_proxy_delete>(deleter);

  // Free it and reset pointer.
  fn(*pointer);
  *pointer = nullptr;
}


LuaTypeProxy::LuaTypeProxy(lua_State* state) {
  this->_state = state;
}

void* LuaTypeProxy::check() const {
  lua_State* state = this->state();

  // Check argument type.
  std::string name = this->typeId();
  void* block = luaL_checkudata(state, 1, name.c_str());

  // Access and return the instance.
  void** pointer = reinterpret_cast<void**>(block);
  return *pointer;
}

lua_State* LuaTypeProxy::state() const {
  if (this->_state == nullptr) {
    throw LuaInvalidProxyUse("LuaTypeProxy::state not set");
  }
  return this->_state;
}


LuaTypeProxy::LuaTypeProxy() {
  this->_state = nullptr;
}

LuaTypeProxy::~LuaTypeProxy() {
  // NOOP.
}

void LuaTypeProxy::initType(Lua lua) {
  std::string name  = this->typeId();
  lua_State*  state = lua.state.get();

  // Check a delete is provided.
  lua_proxy_delete deleter = this->deleter();
  if (deleter == nullptr) {
    throw LuaRuntimeError(
        "Unable to initialise type '" + name +
        "' without a garbage collector method."
    );
  }

  // Create the new type metatable.
  int success = luaL_newmetatable(state, name.c_str());
  if (success == 0) {
    throw LuaTypeExists(name);
  }

  // Register the methods in an __index table.
  lua.stack()->push("__index");
  lua.stack()->newTable();
  luaL_setfuncs(state, this->library(), 0);
  lua_settable(state, -3);

  // Register the __gc closure.
  lua.stack()->push("__gc");
  lua.stack()->pushLight(this->deleter());
  lua.stack()->push(LuaTypeProxy::lua_proxy_gc, 1);
  lua_settable(state, -3);

  // Pop the metatable.
  lua.stack()->remove(-1);
}

void LuaTypeProxy::bind(Lua lua, void* instance) {
  std::string name  = this->typeId();
  lua_State*  state = lua.state.get();

  // Allocate userdata.
  void*  block   = lua_newuserdata(state, sizeof(void*));
  void** pointer = reinterpret_cast<void**>(block);
  *pointer = nullptr;

  // Set the metatable.
  luaL_getmetatable(state, name.c_str());
  lua_setmetatable(state, -2);

  // Store the instance in the userdata.
  *pointer = instance;
}
