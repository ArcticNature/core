// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/client/api.h"

#include "core/context/client.h"
#include "core/context/static.h"
#include "core/model/event.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_service.pb.h"

#include "core/utility/lua/proxy.h"
#include "core/utility/protobuf.h"


using sf::core::bin::ServiceLuaBinding;
using sf::core::bin::ServiceLuaType;

using sf::core::context::Client;
using sf::core::context::Static;
using sf::core::model::EventDrainRef;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::ServiceStart;

using sf::core::utility::Lua;
using sf::core::utility::LuaTypeProxy;
using sf::core::utility::MessageIO;


void ServiceLuaBinding::start(std::string service, std::string version) {
  // Build start request message.
  Message message;
  message.set_code(Message::ServiceStart);
  ServiceStart* req = message.MutableExtension(ServiceStart::msg);
  req->set_service_id(service);
  req->set_version(version);

  // Send the message.
  EventDrainRef drain = Static::drains()->get(Client::server()->id());
  MessageIO<Message>::send(drain, message);
}


const struct luaL_Reg ServiceLuaType::lib[] = {
  {"start", ServiceLuaType::start},
  {nullptr,  nullptr}
};


int ServiceLuaType::start(lua_State* state) {
  Lua* lua = Lua::fetchFrom(state);
  ServiceLuaType type(state);
  int nargs = lua->stack()->size();

  // Check arguments.
  ServiceLuaBinding* service = type.check<ServiceLuaBinding>();
  std::string serv_id = lua->stack()->arguments()->string(2);
  std::string version = lua->stack()->arguments()->string(3);

  // Call method.
  service->start(serv_id, version);
  return 0;
}


void ServiceLuaType::deleteInstance(void* instance) {
  ServiceLuaBinding* value = reinterpret_cast<ServiceLuaBinding*>(instance);
  delete value;
}


ServiceLuaType::ServiceLuaType() : LuaTypeProxy() {
  // NOOP.
}

ServiceLuaType::ServiceLuaType(lua_State* state) : LuaTypeProxy(state) {
  // NOOP.
}


sf::core::utility::lua_proxy_delete ServiceLuaType::deleter() const {
  return ServiceLuaType::deleteInstance;
}

const luaL_Reg* ServiceLuaType::library() const {
  return ServiceLuaType::lib;
}

std::string ServiceLuaType::typeId() const {
  return "ltp::ServiceLuaBinding";
}
