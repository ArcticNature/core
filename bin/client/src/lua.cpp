// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/context/client.h"
#include "core/event/source/readline.h"

#include "core/interface/lifecycle.h"
#include "core/utility/lua.h"

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


//! Handler for client::lua::init that sets global variables.
class ClientLuaInitHandler : public BaseLifecycleHandler {
 public:
  void handle(std::string event, BaseLifecycleArg* argument) {
    Lua lua = Client::lua();

    // Register global functions.
    lua.globals()->set("clear", lua_clear_screen);

    // Register global objects.
    // TODO(stefano): build and register client object.
  }
};


// Static registration of handlers.
LifecycleStaticOn("client::lua::init", ClientLuaInitHandler);
