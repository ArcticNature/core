// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/client/input.h"
#include "core/context/context.h"
#include "core/context/static.h"
#include "core/event/source/manual.h"

#include "core/model/event.h"
#include "core/model/logger.h"
#include "core/lifecycle/event.h"

#include "core/protocols/public/p_client.pb.h"
#include "core/protocols/public/p_message.pb.h"
#include "core/registry/event/handler/api.h"

using sf::core::bin::EnableReadline;
using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::ManualSource;

using sf::core::lifecycle::EventLifecycle;
using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::LogInfo;

using sf::core::protocol::public_api::ClientIntroduce;
using sf::core::protocol::public_api::Message;
using sf::core::registry::ApiHandlerRegistry;


//! Event to handle clients introduction responses.
class ClientIntroduceResponse : public Event {
 protected:
  std::string client_id;
  std::string node_name;

 public:
  ClientIntroduceResponse(
      std::string correlation, std::string drain,
      std::string client_id, std::string node_name
  ) : Event(correlation, drain) {
    this->client_id = client_id;
    this->node_name = node_name;
  }

  void handle() {
    LogInfo info = {
      {"client_id", this->client_id},
      {"node_name", this->node_name}
    };
    DEBUGV(
        Context::logger(),
        "Received id ${client_id} from node ${node_name}.",
        info
    );

    // TODO(stefano): Set client id and node name.

    // Enqueue async readline enable.
    ManualSource* manual = Context::sourceManager()->get<
      ManualSource
    >("manual");
    EventRef async(new EnableReadline());
    EventLifecycle::Init(async);
    manual->enqueueEvent(async);
  }
};


//! Event factory for Introduce responses.
EventRef client_introduce(Message message, std::string drain) {
  ClientIntroduce intro = message.GetExtension(ClientIntroduce::msg);
  return EventRef(new ClientIntroduceResponse(
      message.correlation_id(), drain,
      intro.client_id(), intro.node_name()
  ));
}


// Register event factory.
StaticFactory(ApiHandlerRegistry, "Res::Introduce", client_introduce);
