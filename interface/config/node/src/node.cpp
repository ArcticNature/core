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
using sf::core::utility::lua_proxy_delete;


NodeConfigIntent::NodeConfigIntent(std::string id) : _id(id) {
  // Noop.
}

const std::string NodeConfigIntent::id() const {
  return this->_id;
}


void NodeConfigLoader::apply() {
  std::deque<NodeConfigIntentRef>::iterator it;
  for (it = this->intents.begin(); it != this->intents.end(); it++) {
    NodeConfigIntentRef ref = *it;
    ref->apply(this->new_context);
  }
}

void NodeConfigLoader::initialise() {
  NodeConfigIntentLuaProxy type;
  type.initType(this->lua);

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
  // Visit all intents and sort them by dependency.
  std::deque<NodeConfigIntentRef> intents = this->intents;
  std::deque<NodeConfigIntentRef> sorted;
  std::set<std::string> pending;
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

    // Pre-pend any new dependencies.
    std::vector<std::string> depends = intent->depends();
    if (!depends.empty()) {
      bool pushed = false;
      std::vector<std::string>::iterator it;

      for (it = depends.begin(); it != depends.end(); it++) {
        if (pending.find(*it) != pending.end()) {
          throw InvalidConfiguration(
            "Circular dependency in feature configuration."
          );
        }
        if (visited.find(*it) == visited.end()) {
          pushed = true;
          intents.push_front(this->providers.at(*it));
        }
      }

      // Delay current intent if any dep was pushed.
      if (pushed) {
        continue;
      }
    }

    // Move the intent to the sorted pile.
    intents.pop_front();
    sorted.push_back(intent);
    pending.erase(id);
    visited.insert(id);
  }

  this->intents = sorted;
}

void NodeConfigLoader::updateSystem() {
  Context::initialise(this->new_context);
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
  this->initLuaTables();
  this->initLuaFunctions();
  this->initLuaPatches();

  // Dynamic configuration done by handlers.
  NodeConfigLifecycleArg arg(this, &this->lua);
  Lifecycle::trigger("config::node::init-lua", &arg);
}

void NodeConfigLoader::initLuaFunctions() {
  this->lua.doString("loggers.console = function() end");
  this->lua.doString("sources.scheduler = function() end");
  this->lua.doString("sources.tcp = function() end");
  this->lua.doString("events_from = function() end");
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
  //
}


NodeConfigLoader::NodeConfigLoader(std::string symbolic) {
  this->loaded = false;
  this->symbolic = symbolic;
}

void NodeConfigLoader::addIntent(NodeConfigIntentRef intent) {
  std::string provides = intent->id();
  if (provides != "") {
    this->providers[provides] = intent;
  }
  this->intents.push_back(intent);
}

void NodeConfigLoader::load() {
  if (this->loaded) {
    return;
  }

  this->loaded = true;
  this->initialise();
  this->run();
  this->collectIntents();
  this->sort();
  this->verify();
  this->apply();
  this->updateSystem();
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
