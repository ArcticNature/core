// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/client.h"

#include <string>
#include <memory>

#include "core/context/static.h"
#include "core/model/event.h"
#include "core/utility/lua.h"

using sf::core::context::Client;
using sf::core::context::Static;

using sf::core::model::EventDrainRef;
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

EventDrainRef Client::server() {
  Client::ensureInstance();
  return Static::drains()->get(Client::_instace->server_drain);
}

void Client::server(std::string drain_id) {
  Client::ensureInstance();
  Client::_instace->server_drain = drain_id;
}
