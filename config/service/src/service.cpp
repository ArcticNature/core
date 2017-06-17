// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/config/service.h"

#include <poolqueue/Promise.hpp>
#include <string>

#include "core/config/base.h"
#include "core/config/detail/service_step.h"

#include "core/exceptions/configuration.h"
#include "core/utility/lua.h"
#include "core/utility/lua/table.h"


using poolqueue::Promise;

using sf::core::config::ConfigLoader_;
using sf::core::config::ConfigStep;
using sf::core::config::ServiceDescriptionStep;
using sf::core::config::ServiceLoader;

using sf::core::exception::InvalidConfiguration;
using sf::core::exception::MissingConfiguration;

using sf::core::utility::Lua;
using sf::core::utility::LuaArguments;
using sf::core::utility::LuaTable;
using sf::core::utility::LuaUpvalues;


// Global `service` partial and its wrapper.
/*
 * LUA equivalent:
 *
 *  function service(service_id)
 *    function nested(conf)
 *      if __service then
 *        __service = {
 *          id = service_id,
 *          conf = conf
 *        }
 *      else
 *        error('A service was already defined')
 *      end
 *    end
 *    return nested
 *  end
 */
int lua_service_wrapper(lua_State* state) {
  Lua* lua = Lua::fetchFrom(state);
  LuaArguments args(lua);
  LuaUpvalues upvals(lua);

  // Check arg1 is a table.
  args.checkType(1, LUA_TTABLE);

  // Make sure no other service is defined.
  lua->globals()->toStack("__service");
  int type = lua->stack()->type(-1);
  lua->stack()->remove(-1);
  if (type == LUA_TTABLE) {
    throw InvalidConfiguration("Can only define one service");
  }

  // Build table to store service data.
  LuaTable service = lua->stack()->newTable();
  lua->globals()->fromStack("__service");
  upvals.push(1);
  service.fromStack("id");
  args.push(1);
  service.fromStack("conf");
  return 0;
}
int lua_service_global(lua_State* state) {
  // Check arg1 is a string.
  Lua* lua = Lua::fetchFrom(state);
  LuaArguments args(lua);
  args.checkType(1, LUA_TSTRING);

  // Return a closure on it.
  args.push(1);
  lua->stack()->push(lua_service_wrapper, 1);
  return 1;
}


ServiceLoader::ServiceLoader(
    std::string script, std::string version
) : script_(script), version_(version) {
  // Noop.
}

void ServiceLoader::initLuaEnv() {
  ConfigLoader_::initLuaEnv();
  auto globals = this->lua_.globals();
  auto stack = this->lua_.stack();

  // Add `service` function.
  stack->push(lua_service_global, 0);
  globals->fromStack("service");
}

ConfigStep ServiceLoader::collectStep() {
  auto globals = this->lua_.globals();
  auto stack = this->lua_.stack();

  // Check service data is set and is a table.
  globals->toStack("__service");
  int service_type = stack->type(-1);
  stack->remove(-1);
  if (service_type == LUA_TNONE || service_type == LUA_TNIL) {
    throw MissingConfiguration("No service defined");
  }
  if (service_type != LUA_TTABLE) {
    throw InvalidConfiguration("Invalid service format");
  }

  // Return a config step that processes the service definition.
  auto self = this->shared_from_this();
  return std::make_shared<ServiceDescriptionStep>(self, this->version_);
}

Promise ServiceLoader::runLua() {
  return Promise().settle().then([this]() {
    this->lua_.doString(this->script_);
    return nullptr;
  });
}
