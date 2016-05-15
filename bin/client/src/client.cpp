// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/client.h"

#include <string>

#include "core/bin/client/input.h"
#include "core/context/context.h"

#include "core/event/client/unix.h"
#include "core/event/drain/fd.h"
#include "core/event/source/manual.h"

#include "core/lifecycle/client.h"
#include "core/lifecycle/event.h"
#include "core/model/event.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/utility/protobuf.h"

using sf::core::bin::Client;
using sf::core::bin::EnableReadline;
using sf::core::context::Context;

using sf::core::event::FdDrain;
using sf::core::event::ManualSource;
using sf::core::event::UnixClient;

using sf::core::lifecycle::ClientLifecycle;
using sf::core::lifecycle::EventLifecycle;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;

using sf::core::protocol::public_api::Message;
using sf::core::utility::MessageIO;


class ApiFdDrain : public FdDrain {
 public:
  ApiFdDrain(int fd, std::string id) : FdDrain(fd, id) {}

  void sendAck() {
    Message message;
    message.set_code(Message::Ack);
    MessageIO<Message>::send(this->getFD(), message);
  }
};


class ApiClient : public UnixClient {
 public:
  static std::string Connect(std::string path) {
    return UnixClient::Connect<ApiClient, ApiFdDrain>(path, "api");
  }

  ApiClient(int fd, std::string id, std::string drain_id)
    : UnixClient(fd, id, drain_id) {}

  EventRef parse() {
    if (!this->checkFD()) {
      return EventRef();
    }

    // TODO(stefano): parse event from the channel.
    return EventRef();
  }
};


void Client::connectToServer() {
  std::string path = "/tmp/snow-fox.socket";
  ApiClient::Connect(path);
}


void Client::initialise() {
  // Initialise event sources.
  this->registerDefaultSourceManager();
  Context::sourceManager()->addSource(EventSourceRef(
        new ManualSource()
  ));

  // Initialise client.
  this->maskSignals();
  this->connectToServer();
  ClientLifecycle::Lua::Init();

  // Enqueue async readline enable.
  ManualSource* manual = Context::sourceManager()->get<ManualSource>("manual");
  EventRef async(new EnableReadline());
  EventLifecycle::Init(async);
  manual->enqueueEvent(async);
}
