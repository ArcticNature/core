// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/lifecycle/client.h"

#include "core/interface/lifecycle.h"

using sf::core::interface::Lifecycle;
using sf::core::lifecycle::ClientLifecycle;


void ClientLifecycle::Lua::Init() {
  Lifecycle::trigger("client::lua::init");
}
