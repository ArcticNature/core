// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/cluster/node.h"
#include "core/context/static.h"

#include "core/lifecycle/event.h"
#include "core/model/event.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_node_info.pb.h"

#include "core/registry/event/handler/api.h"
#include "core/utility/protobuf.h"

using sf::core::cluster::Node;
using sf::core::context::Static;

using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::lifecycle::EventLifecycle;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::NodeInfoResponse;
using sf::core::protocol::public_api::NodeInfoRequest;

using sf::core::registry::ApiHandlerRegistry;
using sf::core::utility::MessageIO;


class NodeInfoEvent : public Event {
 protected:
  bool details;

 public:
  NodeInfoEvent(
      bool details, std::string correlation, std::string drain
  ) : Event(correlation, drain) {
    this->details = details;
  }

  void handle() {
    Node* me = Node::me();
    Message message;
    NodeInfoResponse* res = message.MutableExtension(NodeInfoResponse::msg);

    // Wrapper message.
    message.set_code(Message::NodeInfoResponse);
    message.set_correlation_id(this->correlation());

    // Basic node info.
    NodeInfoResponse::NodeInfo* info = res->mutable_node();
    NodeInfoResponse::NodeVersion* ver = info->mutable_version();
    info->set_name(me->name());
    ver->set_commit(me->version().commit);
    ver->set_taint(me->version().taint);
    ver->set_version(me->version().version);

    // Send response to drain.
    int fd = Static::drains()->get(this->drain())->getFD();
    MessageIO<Message>::send(fd, message);
  }
};

EventRef node_info_request(Message message, std::string drain) {
  return EventLifecycle::make<NodeInfoEvent>(
      message.GetExtension(NodeInfoRequest::msg).details(),
      message.correlation_id(), drain
  );
}

StaticFactory(ApiHandlerRegistry, "Req::NodeInfoRequest", node_info_request);
