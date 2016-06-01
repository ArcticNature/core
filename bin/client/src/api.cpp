// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/client/api.h"

#include <string>

#include "core/exceptions/base.h"
#include "core/compile-time/version.h"
#include "core/context/static.h"

#include "core/utility/lua.h"
#include "core/utility/lua/table.h"
#include "core/utility/lua/proxy.h"

using sf::core::bin::ClientLuaBinding;
using sf::core::bin::ClientLuaType;
using sf::core::context::Static;
using sf::core::exception::CleanExit;

using sf::core::utility::Lua;
using sf::core::utility::LuaTable;
using sf::core::utility::LuaTypeProxy;
using sf::core::utility::lua_proxy_delete;


void ClientLuaBinding::exit() {
  throw CleanExit();
}


const struct luaL_Reg ClientLuaType::lib[] = {
  {"exit",    ClientLuaType::exit},
  {"id",      ClientLuaType::id},
  {"version", ClientLuaType::version},
  {nullptr, nullptr}
};

void ClientLuaType::deleteInstance(void* instance) {
  ClientLuaBinding* value = reinterpret_cast<ClientLuaBinding*>(instance);
  delete value;
}

int ClientLuaType::exit(lua_State* state) {
  ClientLuaType type(state);
  ClientLuaBinding* client = type.check<ClientLuaBinding>();
  client->exit();
  return 0;
}

int ClientLuaType::id(lua_State* state) {
  Lua* lua = Lua::fetchFrom(state);
  lua->stack()->push(Static::options()->getString("client-id"));
  return 1;
}

int ClientLuaType::version(lua_State* state) {
  Lua* lua = Lua::fetchFrom(state);
  LuaTable table = lua->stack()->newTable();
  table.set("number", VERSION_NUMBER);
  table.set("hash",   VERSION_SHA);
  table.set("taint",  VERSION_TAINT);
  return 1;
}


ClientLuaType::ClientLuaType(lua_State* state) : LuaTypeProxy(state) {
  // NOOP.
}

std::string ClientLuaType::typeId() const {
  return "ltp::ClientLuaBinding";
}

lua_proxy_delete ClientLuaType::deleter() const {
  return ClientLuaType::deleteInstance;
}

const luaL_Reg* ClientLuaType::library() const {
  return ClientLuaType::lib;
}

ClientLuaType::ClientLuaType() : LuaTypeProxy() {
  // NOOP.
}
