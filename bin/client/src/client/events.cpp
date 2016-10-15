// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/client/events.h"

#include <string>

#include "core/context/client.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/model/event.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_node_info.pb.h"
#include "core/utility/lua/registry.h"
#include "core/utility/protobuf.h"

using sf::core::bin::NodeStatusRequest;
using sf::core::bin::NodeStatusContext;
using sf::core::bin::NSClientContext;
using sf::core::bin::NSClientContextRef;

using sf::core::context::Client;
using sf::core::context::Static;

using sf::core::exception::SfException;
using sf::core::model::Event;
using sf::core::model::EventDrainRef;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::NodeInfoRequest;
using sf::core::utility::MessageIO;


NSClientContext::NSClientContext() {
  this->callback_ref = -1;
}

NSClientContext::~NSClientContext() {
  if (this->callback_ref != -1) {
    Client::lua().registry()->freeReference(this->callback_ref);
  }
}


NodeStatusRequest::NodeStatusRequest(
    int callback_ref, bool details, std::string drain
) : Event("", drain) {
  this->callback_ref = callback_ref;
  this->details = details;
}

void NodeStatusRequest::handle() {
  // Store callback reference in NodeStatusContext.
  NSClientContextRef context(new NSClientContext());
  context->callback_ref = this->callback_ref;
  NodeStatusContext::store(this->correlation(), context);

  // Build status request.
  Message message;
  message.set_code(Message::NodeInfoRequest);
  message.set_correlation_id(this->correlation());

  NodeInfoRequest* info = message.MutableExtension(NodeInfoRequest::msg);
  info->set_details(this->details);

  // Send status request.
  EventDrainRef drain = Static::drains()->get(this->drain());
  MessageIO<Message>::send(drain->getFD(), message);
}
