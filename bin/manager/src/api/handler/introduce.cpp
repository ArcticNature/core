// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/cluster/node.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/protocols/public/p_client.pb.h"
#include "core/protocols/public/p_message.pb.h"
#include "core/registry/event/handler/api.h"
#include "core/utility/protobuf.h"

using sf::core::cluster::Node;
using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::LogInfo;

using sf::core::protocol::public_api::ClientIntroduce;
using sf::core::protocol::public_api::Message;
using sf::core::registry::ApiHandlerRegistry;
using sf::core::utility::MessageIO;


//! Event to handle clients introduction requests.
class ClientIntroduceEvent : public Event {
 public:
  ClientIntroduceEvent(
      std::string correlation, std::string drain
  ) : Event(correlation, drain) {
    // NOOP.
  }

  void handle() {
    // Generate unique id for the client.
    std::string node_name = Node::me()->name();
    std::string client_id = "CLI_NUM@" + node_name + "!PID";
    LogInfo info = {{"client_id", client_id}};
    DEBUGV(Context::logger(), "Welcoming new client: '${client_id}'", info);

    // Build client response.
    Message response;
    ClientIntroduce* intro = response.MutableExtension(ClientIntroduce::msg);
    response.set_code(Message::Introduce);
    intro->set_client_id(client_id);
    intro->set_node_name(node_name);

    // Send response to client.
    int drain = Static::drains()->get(this->drain_id)->getFD();
    MessageIO<Message>::send(drain, response);
  }
};


//! Event factory for Introduce requests.
EventRef client_introduce(Message message, std::string drain) {
  return EventRef(
      new ClientIntroduceEvent(message.correlation_id(), drain)
  );
}


// Register event factory.
StaticFactory(ApiHandlerRegistry, "Req::Introduce", client_introduce);