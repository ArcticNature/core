// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/client.h"

#include <memory>

#include "core/utility/lua.h"

using sf::core::context::Client;
using sf::core::utility::Lua;


Client::Client() {
  // NOOP.
}

void Client::ensureInstance() {
  if (Client::_instace) {
    return;
  }
  Client::_instace = std::shared_ptr<Client>(new Client());
}


std::shared_ptr<Client> Client::_instace;


Client::~Client() {
  // NOOP.
}

void Client::destroy() {
  Client::_instace = std::shared_ptr<Client>();
}

Lua Client::lua() {
  Client::ensureInstance();
  return Client::_instace->_lua;
}
