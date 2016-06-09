// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/client/api.h"
#include "core/bin/client/events.h"

#include "core/context/client.h"
#include "core/context/context.h"
#include "core/event/source/manual.h"
#include "core/lifecycle/event.h"
#include "core/model/event.h"

#include "core/utility/lua.h"
#include "core/utility/lua/registry.h"

using sf::core::bin::NodeLuaBinding;
using sf::core::bin::NodeLuaType;
using sf::core::bin::NodeStatusRequest;

using sf::core::context::Client;
using sf::core::context::Context;
using sf::core::event::ManualSource;
using sf::core::lifecycle::EventLifecycle;
using sf::core::model::EventRef;

using sf::core::utility::Lua;
using sf::core::utility::LuaTypeProxy;
using sf::core::utility::lua_proxy_delete;


void NodeLuaBinding::status(Lua* lua, int callback_ref, bool details) {
  // Create the reqest event.
  EventRef request = EventLifecycle::make<NodeStatusRequest>(
    callback_ref, details, Client::server()->id()
  );

  // Enqueue it for handling.
  ManualSource* manual = Context::sourceManager()->get<ManualSource>("manual");
  manual->enqueueEvent(request);

  // TODO(stefano): implement process as below.
  // On repsonse convert it to LUA table.
  // Call the callback with the response.

  // TODO(stefano): Testing code to remove.
  lua->registry()->dereference(callback_ref);
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
  int nargs = lua->stack()->size();

  // Check arguments.
  NodeLuaBinding* node = type.check<NodeLuaBinding>();
  lua->stack()->arguments()->any(2);
  int  ref = lua->stack()->arguments()->reference(2);
  bool details = false;
  if (nargs == 3) {
    details = lua->stack()->arguments()->boolean(3);
  }

  // Call method.
  node->status(lua, ref, details);
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
