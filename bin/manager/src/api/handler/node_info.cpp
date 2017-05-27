// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/cluster/cluster.h"
#include "core/cluster/node.h"
#include "core/context/static.h"

#include "core/lifecycle/event.h"
#include "core/model/event.h"
#include "core/model/repository.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_node_info.pb.h"

#include "core/registry/event/handler/api.h"
#include "core/utility/status.h"
#include "core/utility/protobuf.h"

using sf::core::cluster::Cluster;
using sf::core::cluster::Node;
using sf::core::cluster::NodeVersion;

using sf::core::context::Static;

using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::RepositoryVersionId;
using sf::core::lifecycle::EventLifecycle;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::StatusColour;
using sf::core::protocol::public_api::NodeInfoResponse;
using sf::core::protocol::public_api::NodeInfoRequest;

using sf::core::registry::ApiHandlerRegistry;
using sf::core::utility::MessageIO;

using sf::core::utility::SubsystemStatus;
using sf::core::utility::Status;
using sf::core::utility::StatusLight;


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
      Status* status, NodeInfoResponse::SystemStatus* message
  ) {
    message->set_colour(ENUM_TO_PB[status->colour() + 1]);
    message->set_reason(status->message());
  }

 public:
  NodeInfoEvent(
      bool details, std::string correlation, EventDrainRef drain
  ) : Event(correlation, drain) {
    this->details = details;
  }

  void handle() {
    Node myself = Cluster::Instance()->myself();
    SubsystemStatus* node_status = myself->status();
    NodeVersion node_version = myself->version();
    RepositoryVersionId config_version = myself->configVersion();

    // Wrapper message.
    Message message;
    message.set_code(Message::NodeInfoResponse);
    message.set_correlation_id(this->correlation());
    NodeInfoResponse* res = message.MutableExtension(NodeInfoResponse::msg);

    // Basic node info.
    NodeInfoResponse::NodeInfo* info = res->mutable_node();
    NodeInfoResponse::NodeVersion* ver = info->mutable_version();
    NodeInfoResponse::ConfigVersion* config = info->mutable_config();
    info->set_name(myself->name());
    ver->set_commit(node_version.commit);
    ver->set_taint(node_version.taint);
    ver->set_version(node_version.version);
    config->set_effective(config_version.effective());
    config->set_symbolic(config_version.symbolic());

    // Node status info.
    this->statusToMessage(
        node_status, res->mutable_overall()
    );

    // Node status details.
    if (this->details) {
      for (auto pair : *node_status) {
        NodeInfoResponse::SubSystem* sub = res->add_details();
        sub->set_name(pair.first);
        this->statusToMessage(&pair.second, sub->mutable_status());
      }
    }

    // Send response to drain.
    EventDrainRef drain = this->drain();
    MessageIO<Message>::send(drain, message);
  }
};

EventRef node_info_request(Message message, EventDrainRef drain) {
  return EventLifecycle::make<NodeInfoEvent>(
      message.GetExtension(NodeInfoRequest::msg).details(),
      message.correlation_id(), drain
  );
}

StaticFactory(ApiHandlerRegistry, "Req::NodeInfoRequest", node_info_request);
