// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/config/node.h"

#include <deque>
#include <stack>
#include <set>
#include <string>
#include <vector>

#include "core/cluster/node.h"
#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/configuration.h"

#include "core/model/logger.h"
#include "core/model/repository.h"
#include "core/utility/lua/proxy.h"

using sf::core::cluster::Node;
using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::context::Static;

using sf::core::exception::InvalidConfiguration;
using sf::core::exception::MissingConfiguration;

using sf::core::interface::Lifecycle;
using sf::core::interface::NodeConfigIntent;
using sf::core::interface::NodeConfigIntentLuaProxy;
using sf::core::interface::NodeConfigIntentRef;
using sf::core::interface::NodeConfigLoader;
using sf::core::lifecycle::NodeConfigLifecycleArg;

using sf::core::model::IStreamRef;
using sf::core::model::RepositoryVersionId;
using sf::core::utility::Lua;
using sf::core::utility::LuaTable;
using sf::core::utility::LuaUpvalues;
using sf::core::utility::lua_proxy_delete;


NodeConfigIntent::NodeConfigIntent(std::string id) : _id(id) {
  // Noop.
}

std::vector<std::string> NodeConfigIntent::after() const {
  std::vector<std::string> none;
  return none;
}

const std::string NodeConfigIntent::id() const {
  return this->_id;
}


int NodeConfigLoader::lua_events_from(lua_State* state) {
  Lua* lua = Lua::fetchFrom(state);
  lua->stack()->arguments()->any(1);
  bool added = false;

  // If a function is passed execute it and use the result.
  if (lua->stack()->type(1) == LUA_TFUNCTION) {
    DEBUG(
        Context::Logger(),
        "Resolving function passed to 'core.events_from' ..."
    );
    lua->call(0, 1);
  }

  // Extract the intent.
  NodeConfigIntentLuaProxy proxy(*lua);
  if (proxy.typeOf(1)) {
    LuaUpvalues upvalues = lua->stack()->upvalues();
    NodeConfigLoader* loader = upvalues.toLightUserdata<NodeConfigLoader>(1);
    loader->addIntent(proxy.get(1));
    added = true;

  } else {
    DEBUG(
        Context::Logger(),
        "Skipping argument passed to 'core.events_from' due to incorrect type."
    );
  }

  lua->stack()->pushBool(added);
  return 1;
}


void NodeConfigLoader::apply() {
  std::deque<NodeConfigIntentRef>::iterator it;
  for (it = this->intents.begin(); it != this->intents.end(); it++) {
    NodeConfigIntentRef ref = *it;
    ref->apply(this->new_context);
  }
}

void NodeConfigLoader::initialise() {
  this->initLua();
  this->new_context = ContextRef(new Context());
  this->effective = Static::repository()->resolveVersion(this->symbolic);
  this->repo_ver = Static::repository()->lookup(this->effective);
}

std::vector<std::string> NodeConfigLoader::paths() const {
  std::string node_name = Node::me()->name();

  std::vector<std::string> result;
  std::vector<std::string>::iterator it;
  std::vector<std::string> options = {
    "config/main.lua",
    "config/" + node_name + "/main.lua"
  };

  for (it = options.begin(); it != options.end(); it++) {
    if (this->repo_ver->exists(*it)) {
      result.push_back(*it);
    }
  }
  return result;
}

void NodeConfigLoader::run() {
  const std::vector<std::string> files = this->paths();
  std::vector<std::string>::const_iterator it;
  for (it = files.cbegin(); it != files.cend(); it++) {
    IStreamRef stream = this->repo_ver->streamFile(*it);
    this->lua.doStream(stream, *it);
  }
}

void NodeConfigLoader::sort() {
  DEBUG(Context::Logger(), "Sorting node configuration intents ...");

  // Visit all intents and sort them by dependency.
  std::deque<NodeConfigIntentRef> intents = this->intents;
  std::deque<NodeConfigIntentRef> sorted;
  std::set<std::string> pending;
  std::set<std::string> provided;
  std::set<std::string> visited;

  while (!intents.empty()) {
    NodeConfigIntentRef intent = intents[0];
    std::string id = intent->id();

    // Ignore intents we processed already.
    if (visited.find(id) != visited.end()) {
      intents.pop_front();
      continue;
    }
    pending.insert(id);

    // Pre-pend any new dependencies/optional dependencies.
    bool pushed = false;
    std::vector<std::string> before_me = intent->after();
    std::vector<std::string> depends   = intent->depends();

    if (!depends.empty()) {
      std::vector<std::string>::iterator it;
      for (it = depends.begin(); it != depends.end(); it++) {
        // Check that a provider exists.
        this->requireProvider(*it);
        NodeConfigIntentRef provided_by = this->providers.at(*it);

        // Make sure there are no loops.
        if (pending.find(provided_by->id()) != pending.end()) {
          throw InvalidConfiguration(
            "Circular dependency in feature configuration."
          );
        }

        // Enqueue needed dependencies.
        if (visited.find(provided_by->id()) == visited.end()) {
          pushed = true;
          intents.push_front(provided_by);
        }
      }
    }

    if (!before_me.empty()) {
      std::vector<std::string>::iterator it;
      for (it = before_me.begin(); it != before_me.end(); it++) {
        // Check if the provider is registered.
        if (this->providers.find(*it) == this->providers.end()) {
          continue;
        }

        // Make sure there are no loops.
        NodeConfigIntentRef provided_by = this->providers.at(*it);
        if (pending.find(provided_by->id()) != pending.end()) {
          throw InvalidConfiguration(
            "Circular dependency in feature configuration."
          );
        }

        // Enqueue needed dependencies.
        if (visited.find(provided_by->id()) == visited.end()) {
          pushed = true;
          intents.push_front(provided_by);
        }
      }
    }

    // Delay current intent if any dependency was pushed.
    if (pushed) {
      continue;
    }

    // Move the intent to the sorted pile.
    intents.pop_front();
    sorted.push_back(intent);
    pending.erase(id);
    visited.insert(id);
  }

  this->intents = sorted;
  DEBUG(Context::Logger(), "Node configuration intents sorted!");
}

void NodeConfigLoader::updateSystem() {
  Context::Initialise(this->new_context);
  RepositoryVersionId new_version(this->effective, this->symbolic);
  Node::me()->configVersion(new_version);
}

void NodeConfigLoader::verify() {
  std::deque<NodeConfigIntentRef>::iterator it;
  for (it = this->intents.begin(); it != this->intents.end(); it++) {
    NodeConfigIntentRef ref = *it;
    ref->verify(this->new_context);
  }

  // Check that required options are provided.
  this->requireProvider("event.manager");
}


void NodeConfigLoader::collectIntents() {
  NodeConfigLifecycleArg arg(this, &this->lua);
  Lifecycle::trigger("config::node::collect", &arg);
}

void NodeConfigLoader::initLua() {
  // Essential lua configuration that Lifecycle handlers may relay on.
  NodeConfigIntentLuaProxy type;
  type.initType(this->lua);

  this->initLuaTables();
  this->initLuaFunctions();
  this->initLuaPatches();

  // Dynamic configuration done by handlers.
  NodeConfigLifecycleArg arg(this, &this->lua);
  Lifecycle::trigger("config::node::init-lua", &arg);
}

void NodeConfigLoader::initLuaFunctions() {
  LuaTable core = this->lua.globals()->toTable("core");
  this->lua.stack()->pushLight<NodeConfigLoader>(this);
  this->lua.stack()->push(NodeConfigLoader::lua_events_from, 1);
  core.fromStack("events_from");
}

void NodeConfigLoader::initLuaTables() {
  std::vector<std::string>::iterator it;
  std::vector<std::string> tables = {
    "connectors",
    "core",
    "event_managers",
    "loggers",
    "sources"
  };

  for (it = tables.begin(); it != tables.end(); it++) {
    this->lua.stack()->newTable();
    this->lua.globals()->fromStack(*it);
  }
}

void NodeConfigLoader::initLuaPatches() {
  this->lua.doString("loggers.console = function() return nil end");
  this->lua.doString("sources.tcp = function() return nil end");
  this->lua.doString("sources.unix = function() return nil end");
}


NodeConfigLoader::NodeConfigLoader(std::string symbolic) {
  this->loaded = false;
  this->symbolic = symbolic;
}

void NodeConfigLoader::addIntent(NodeConfigIntentRef intent) {
  std::string provides = intent->provides();
  if (provides != "") {
    this->providers[provides] = intent;
  }
  this->intents.push_back(intent);
}

void NodeConfigLoader::load() {
  if (this->loaded) {
    return;
  }

  this->initialise();
  this->run();
  this->collectIntents();
  this->sort();
  this->verify();
  this->apply();
  this->updateSystem();
  this->loaded = true;
}

void NodeConfigLoader::requireProvider(std::string id) {
  if (this->providers.find(id) == this->providers.end()) {
    throw MissingConfiguration(
        "Could not find a configured provider '" + id + "'."
    );
  }
}


NodeConfigIntentLuaProxy::NodeConfigIntentLuaProxy() : LuaTypeProxy() {
  // Noop.
}

NodeConfigIntentLuaProxy::NodeConfigIntentLuaProxy(
    Lua lua
) : LuaTypeProxy(lua) {
  // Noop.
}

const luaL_Reg NodeConfigIntentLuaProxy::lib[] = {{nullptr, nullptr}};

void NodeConfigIntentLuaProxy::delete_instance(void* instance) {
  NodeConfigIntentRef* ref = reinterpret_cast<NodeConfigIntentRef*>(instance);
  delete ref;
}

std::string NodeConfigIntentLuaProxy::typeId() const {
  return "ltp:NodeConfigIntentRef";
}

lua_proxy_delete NodeConfigIntentLuaProxy::deleter() const {
  return NodeConfigIntentLuaProxy::delete_instance;
}

const luaL_Reg* NodeConfigIntentLuaProxy::library() const {
  return NodeConfigIntentLuaProxy::lib;
}


NodeConfigIntentRef NodeConfigIntentLuaProxy::get(int idx, bool pop) {
  // Check argument type and fetch pointer.
  std::string name = this->typeId();
  void* block = luaL_checkudata(this->state(), idx, name.c_str());
  void** pointer = reinterpret_cast<void**>(block);
  if (pop) {
    lua_remove(this->state(), idx);
  }

  // Create ref copy.
  NodeConfigIntentRef* ref = reinterpret_cast<NodeConfigIntentRef*>(*pointer);
  return NodeConfigIntentRef(*ref);
}

void NodeConfigIntentLuaProxy::wrap(Lua lua, NodeConfigIntent* intent) {
  this->wrap(lua, NodeConfigIntentRef(intent));
}

void NodeConfigIntentLuaProxy::wrap(Lua lua, NodeConfigIntentRef intent) {
  NodeConfigIntentRef* ref = new NodeConfigIntentRef(intent);
  this->bind(lua, ref);
}


NodeConfigLifecycleArg::NodeConfigLifecycleArg(
    NodeConfigLoader* loader, Lua* lua
) {
  this->_loader = loader;
  this->_lua = lua;
}

NodeConfigLoader* NodeConfigLifecycleArg::loader() const {
  return this->_loader;
}

Lua* NodeConfigLifecycleArg::lua() const {
  return this->_lua;
}
