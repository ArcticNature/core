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
#include "core/utility/string.h"

using sf::core::cluster::Node;
using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::LogInfo;

using sf::core::protocol::public_api::ClientIntroduce;
using sf::core::protocol::public_api::Message;
using sf::core::registry::ApiHandlerRegistry;

using sf::core::utility::MessageIO;
using sf::core::utility::string::toString;


//! Event to handle clients introduction requests.
class ClientIntroduceEvent : public Event {
 protected:
  static uint64_t clients_count;

 public:
  ClientIntroduceEvent(
      std::string correlation, EventDrainRef drain
  ) : Event(correlation, drain) {
    // NOOP.
  }

  void handle() {
    // Generate unique id for the client.
    std::string node_name = Node::me()->name();
    std::string num = toString(ClientIntroduceEvent::clients_count++);
    std::string pid = toString(Static::posix()->getpid());
    std::string client_id = num + "@" + node_name + "!" + pid;
    LogInfo info = {{"client_id", client_id}};
    DEBUGV(Context::Logger(), "Welcoming new client: '${client_id}'", info);

    // Build client response.
    Message response;
    ClientIntroduce* intro = response.MutableExtension(ClientIntroduce::msg);
    response.set_code(Message::Introduce);
    intro->set_client_id(client_id);
    intro->set_node_name(node_name);

    // Send response to client.
    EventDrainRef drain = this->drain();
    MessageIO<Message>::send(drain, response);
  }
};
uint64_t ClientIntroduceEvent::clients_count = 0;


//! Event factory for Introduce requests.
EventRef client_introduce(Message message, EventDrainRef drain) {
  return EventRef(
      new ClientIntroduceEvent(message.correlation_id(), drain)
  );
}

// Register event factory.
StaticFactory(ApiHandlerRegistry, "Req::Introduce", client_introduce);
