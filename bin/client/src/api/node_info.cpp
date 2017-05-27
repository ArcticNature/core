// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/client/events.h"
#include "core/context/client.h"

#include "core/event/drain/null.h"
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

using sf::core::event::NullDrain;
using sf::core::lifecycle::EventLifecycle;
using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::NodeInfoResponse;
using sf::core::protocol::public_api::StatusColour_Name;
using sf::core::registry::ApiHandlerRegistry;

using sf::core::utility::Lua;
using sf::core::utility::LuaTable;


class NodeInfo : public Event {
 protected:
  Message message;

  void statusToTable(
      LuaTable* table,
      const NodeInfoResponse::SystemStatus& status
  ) {
    table->set("colour", StatusColour_Name(status.colour()));
    table->set("reason", status.reason());
  }

 public:
  NodeInfo(
      std::string correlation, Message message
  ) : Event(correlation, EventDrainRef(new NullDrain)) {
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
      response.fromStack("version");
      response.set("name", info.node().name());
      version.set("commit", node_ver.commit());
      if (node_ver.has_taint()) {
        version.set("taint", node_ver.taint());
      }
      if (node_ver.has_version()) {
        version.set("version", node_ver.version());
      }

      if (info.node().has_config()) {
        const NodeInfoResponse::ConfigVersion& config_ver =
          info.node().config();
        LuaTable config = lua.stack()->newTable();
        response.fromStack("config");
        config.set("effective", config_ver.effective());
        config.set("symbolic", config_ver.symbolic());
      }
    }

    if (info.has_overall()) {
      LuaTable overall = lua.stack()->newTable();
      response.fromStack("overall");
      this->statusToTable(&overall, info.overall());
    }

    if (!info.details().empty()) {
      LuaTable details = lua.stack()->newTable();
      response.fromStack("details");
      int sub_sys = info.details_size();

      for (int idx = 0; idx < sub_sys; idx++) {
        const NodeInfoResponse::SubSystem& sub = info.details(idx);
        LuaTable sub_table = lua.stack()->newTable();
        details.fromStack(sub.name());
        this->statusToTable(&sub_table, sub.status());
      }
    }

    // Call function stored in context.
    NSClientContextRef context = NodeStatusContext::pop(this->correlation());
    lua.registry()->dereference(context->callback_ref);
    response.pushMe();
    lua.call(1);
  }
};

EventRef node_info_res(Message message, EventDrainRef drain) {
  return EventLifecycle::make<NodeInfo>(message.correlation_id(), message);
}

StaticFactory(ApiHandlerRegistry, "Res::NodeInfoResponse", node_info_res);
