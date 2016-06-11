// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/client/events.h"
#include "core/context/client.h"

#include "core/lifecycle/event.h"
#include "core/model/event.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_node_info.pb.h"
#include "core/registry/event/handler/api.h"

#include "core/utility/lua.h"
#include "core/utility/lua/table.h"
#include "core/utility/lua/registry.h"

using sf::core::bin::NodeStatusContext;
using sf::core::bin::NSClientContextRef;
using sf::core::context::Client;

using sf::core::lifecycle::EventLifecycle;
using sf::core::model::Event;
using sf::core::model::EventRef;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::NodeInfoResponse;
using sf::core::registry::ApiHandlerRegistry;

using sf::core::utility::Lua;
using sf::core::utility::LuaTable;


class NodeInfo : public Event {
 protected:
  Message message;

 public:
  NodeInfo(
      std::string correlation, Message message
  ) : Event(correlation, "NULL") {
    this->message = message;
  }

  void handle() {
    // Create empty LUA table.
    Lua lua = Client::lua();
    LuaTable response = lua.stack()->newTable(true);

    // Fill table with data from Message.
    const NodeInfoResponse& info = this->message.GetExtension(
        NodeInfoResponse::msg
    );
    if (info.has_node()) {
      const NodeInfoResponse::NodeVersion& node_ver = info.node().version();
      LuaTable version = lua.stack()->newTable();
      response.set("name", info.node().name());
      response.fromStack("version");
      version.set("commit", node_ver.commit());
      if (node_ver.has_taint()) {
        version.set("taint", node_ver.taint());
      }
      if (node_ver.has_version()) {
        version.set("version", node_ver.version());
      }
    }

    // Call funCtion stored in context.
    NSClientContextRef context = NodeStatusContext::pop(this->correlation());
    lua.registry()->dereference(context->callback_ref);
    response.pushMe();
    lua.call(1);
  }
};

EventRef node_info_res(Message message, std::string drain) {
  return EventLifecycle::make<NodeInfo>(message.correlation_id(), message);
}

StaticFactory(ApiHandlerRegistry, "Res::NodeInfoResponse", node_info_res);