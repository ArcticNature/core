// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/client/api.h"

#include "core/utility/lua.h"
#include "core/utility/lua/registry.h"

using sf::core::bin::NodeLuaBinding;
using sf::core::bin::NodeLuaType;

using sf::core::utility::Lua;
using sf::core::utility::LuaTypeProxy;
using sf::core::utility::lua_proxy_delete;


void NodeLuaBinding::status(Lua* lua, int callback_ref) {
  lua->registry()->dereference(callback_ref);
  lua->registry()->freeReference(callback_ref);
  lua->stack()->push("TODO!!!");
  lua->call(1);
}


const struct luaL_Reg NodeLuaType::lib[] = {
  {"status", NodeLuaType::status},
  {nullptr,  nullptr}
};

void NodeLuaType::deleteInstance(void* instance) {
  NodeLuaBinding* value = reinterpret_cast<NodeLuaBinding*>(instance);
  delete value;
}

int NodeLuaType::status(lua_State* state) {
  Lua* lua = Lua::fetchFrom(state);
  NodeLuaType type(state);

  // Check arguments.
  NodeLuaBinding* node = type.check<NodeLuaBinding>();
  lua->stack()->arguments()->any(2);

  // Call method.
  node->status(lua, lua->stack()->arguments()->reference(2));
  return 0;
}

NodeLuaType::NodeLuaType(lua_State* state) : LuaTypeProxy(state) {
  // NOOP.
}

std::string NodeLuaType::typeId() const {
  return "ltp::NodeLuaBinding";
}

lua_proxy_delete NodeLuaType::deleter() const {
  return NodeLuaType::deleteInstance;
}

const luaL_Reg* NodeLuaType::library() const {
  return NodeLuaType::lib;
}

NodeLuaType::NodeLuaType() : LuaTypeProxy() {
  // NOOP.
}
