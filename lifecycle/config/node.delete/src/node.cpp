// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/lifecycle/config/node.h"

#include "core/interface/lifecycle.h"
#include "core/utility/lua.h"

using sf::core::interface::Lifecycle;
using sf::core::interface::NodeConfigLoader;
using sf::core::lifecycle::NodeConfigLifecycle;
using sf::core::lifecycle::NodeConfigLifecycleArg;
using sf::core::utility::Lua;


void NodeConfigLifecycle::InitLua(NodeConfigLoader* loader, Lua* lua) {
  NodeConfigLifecycleArg arg(loader, lua);
  Lifecycle::trigger("config::node::init-lua", &arg);
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
