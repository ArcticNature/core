// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/config/hooks.h"

#include "core/hook.h"
#include "core/utility/lua.h"


using ConfigLoaderHook = sf::core::hook::ConfigLoader;

using sf::core::config::ConfigLoader;
using sf::core::hook::Hook;
using sf::core::utility::Lua;


Hook<ConfigLoader, Lua> ConfigLoaderHook::InitialiseLua;
