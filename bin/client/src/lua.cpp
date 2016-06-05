// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <iostream>
#include <string>

#include "core/bin/client/api.h"

#include "core/context/client.h"
#include "core/event/source/readline.h"

#include "core/interface/lifecycle.h"
#include "core/utility/lua.h"

using sf::core::bin::ClientLuaBinding;
using sf::core::bin::ClientLuaType;
using sf::core::bin::NodeLuaBinding;
using sf::core::bin::NodeLuaType;

using sf::core::context::Client;
using sf::core::event::ReadlineEventSource;

using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;
using sf::core::utility::Lua;


int lua_clear_screen(lua_State* state) {
  ReadlineEventSource::showPrompt();
  ReadlineEventSource::clearScreen();
  ReadlineEventSource::clearLine();
  return 0;
}

int lua_print(lua_State* state) {
  Lua* lua = Lua::fetchFrom(state);
  lua->stack()->print(&std::cout);
  return 0;
}


//! Handler for client::lua::init that sets global variables.
class ClientLuaInitHandler : public BaseLifecycleHandler {
 public:
  void handle(std::string event, BaseLifecycleArg* argument) {
    Lua lua = Client::lua();

    // Register global functions.
    lua.globals()->set("clear", lua_clear_screen);
    lua.globals()->set("print", lua_print);

    // Register global `client` object.
    ClientLuaBinding* client = new ClientLuaBinding();
    ClientLuaType client_type;
    client_type.initType(lua);
    client_type.bind(lua, client);
    lua.globals()->fromStack("client");

    // Register global `node` object.
    NodeLuaBinding* node = new NodeLuaBinding();
    NodeLuaType node_type;
    node_type.initType(lua);
    node_type.bind(lua, node);
    lua.globals()->fromStack("node");
  }
};


// Static registration of handlers.
LifecycleStaticOn("client::lua::init", ClientLuaInitHandler);
