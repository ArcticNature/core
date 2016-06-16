// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/cluster/node.h"
#include "core/context/static.h"

#include "core/lifecycle/event.h"
#include "core/model/event.h"
#include "core/model/repository.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_node_info.pb.h"

#include "core/registry/event/handler/api.h"
#include "core/utility/protobuf.h"

using sf::core::cluster::Node;
using sf::core::cluster::NodeStatus;
using sf::core::cluster::NodeStatusDetail;
using sf::core::cluster::NodeVersion;
using sf::core::context::Static;

using sf::core::model::Event;
using sf::core::model::EventRef;
using sf::core::model::RepositoryVersionId;
using sf::core::lifecycle::EventLifecycle;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::StatusColour;
using sf::core::protocol::public_api::NodeInfoResponse;
using sf::core::protocol::public_api::NodeInfoRequest;

using sf::core::registry::ApiHandlerRegistry;
using sf::core::utility::MessageIO;


static StatusColour ENUM_TO_PB[] = {
  StatusColour::Unkown,
  StatusColour::Green,
  StatusColour::Yellow,
  StatusColour::Red
};


class NodeInfoEvent : public Event {
 protected:
  bool details;

  void statusToMessage(
      NodeStatusDetail status,
      NodeInfoResponse::SystemStatus* message
  ) {
    message->set_colour(ENUM_TO_PB[status.colour() + 1]);
    message->set_code(status.code());
    message->set_reason(status.message());
  }

 public:
  NodeInfoEvent(
      bool details, std::string correlation, std::string drain
  ) : Event(correlation, drain) {
    this->details = details;
  }

  void handle() {
    Node* me = Node::me();
    NodeStatus* node_status  = me->status();
    NodeVersion node_version = me->version();
    RepositoryVersionId config_version = me->configVersion();

    // Wrapper message.
    Message message;
    message.set_code(Message::NodeInfoResponse);
    message.set_correlation_id(this->correlation());
    NodeInfoResponse* res = message.MutableExtension(NodeInfoResponse::msg);

    // Basic node info.
    NodeInfoResponse::NodeInfo* info = res->mutable_node();
    NodeInfoResponse::NodeVersion* ver = info->mutable_version();
    NodeInfoResponse::ConfigVersion* config = info->mutable_config();
    info->set_name(me->name());
    ver->set_commit(node_version.commit);
    ver->set_taint(node_version.taint);
    ver->set_version(node_version.version);
    config->set_effective(config_version.effective());
    config->set_symbolic(config_version.symbolic());

    // Node status info.
    this->statusToMessage(
        node_status->reason(), res->mutable_overall()
    );

    // Node status details.
    if (this->details) {
      NodeStatus::iterator it;
      for (it = node_status->begin(); it != node_status->end(); it++) {
        NodeInfoResponse::SubSystem* sub = res->add_details();
        sub->set_name(it->first);
        this->statusToMessage(it->second, sub->mutable_status());
      }
    }

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
