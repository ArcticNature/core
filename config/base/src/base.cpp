// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/config/base.h"

#include <poolqueue/Promise.hpp>

#include "core/config/hooks.h"
#include "core/utility/lua.h"


using ConfigLoaderHook = sf::core::hook::ConfigLoader;

using poolqueue::Promise;

using sf::core::config::ConfigLoader_;
using sf::core::config::ConfigStep_;
using sf::core::utility::Lua;


void ConfigLoader_::initialise() {
  this->initLuaEnv();
  ConfigLoader loader = this->shared_from_this();
  ConfigLoaderHook::InitialiseLua.trigger(loader, this->lua_);
  this->initialiseLua.trigger(loader, this->lua_);
}

Promise ConfigLoader_::load() {
  // Initialise and run LUA.
  this->initialise();
  auto load_chain = this->runLua();

  // Collect, verify, and return the step.
  load_chain = load_chain.then([this]() {
      auto step = this->collectStep();
      return step->verify().then([step]() {
          return step;
      });
  });

  // Execute the step and return it.
  return load_chain.then([](ConfigStep step) {
      return step->execute();
  });
}

void ConfigLoader_::initLuaEnv() {
  // Noop.
}


ConfigStep_::ConfigStep_(ConfigLoader loader) {
  this->loader_ = loader;
}

Lua ConfigStep_::lua() {
  return this->loader_->lua_;
}
