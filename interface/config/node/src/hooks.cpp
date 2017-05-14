// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/config/node/hooks.h"


using sf::core::hook::Hook;
using sf::core::hook::NodeConfig;

using sf::core::interface::NodeConfigLoader;
using sf::core::utility::Lua;


Hook<Lua, NodeConfigLoader*> NodeConfig::Collect;
Hook<Lua> NodeConfig::LuaInit;
