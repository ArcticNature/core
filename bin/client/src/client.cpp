// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/client.h"

#include <string>

#include "core/context/client.h"
#include "core/context/context.h"

#include "core/event/client/unix.h"
#include "core/event/drain/fd.h"
#include "core/event/source/manual.h"
#include "core/exceptions/base.h"

#include "core/lifecycle/client.h"
#include "core/lifecycle/event.h"
#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/registry/event/handler/api.h"
#include "core/utility/protobuf.h"

using sf::core::bin::Client;
using sf::core::context::Context;

using sf::core::event::FdDrain;
using sf::core::event::ManualSource;
using sf::core::event::UnixClient;
using sf::core::exception::CorruptedData;

using sf::core::lifecycle::ClientLifecycle;
using sf::core::lifecycle::EventLifecycle;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;

using sf::core::protocol::public_api::Message;
using sf::core::registry::ApiEventFactory;
using sf::core::registry::ApiHandlerRegistry;
using sf::core::utility::MessageIO;


class ApiFdDrain : public FdDrain {
 public:
  ApiFdDrain(int fd, std::string id) : FdDrain(fd, id) {
    // Noop.
  }
};


class ApiClient : public UnixClient {
 public:
  static std::string Connect(std::string path) {
    return UnixClient::Connect<ApiClient, ApiFdDrain>(path, "api").second;
  }

  ApiClient(int fd, std::string id, EventDrainRef drain)
    : UnixClient(fd, id, drain) {
    // NOOP.
  }

  EventRef parse() {
    if (!this->checkFD()) {
      return EventRef();
    }

    // Parse event.
    int fd = this->fd();
    Message message;
    bool parsed = MessageIO<Message>::parse(fd, &message);
    if (!parsed) {
      throw CorruptedData("Unable to parse protocol buffer");
    }

    // Create event.
    std::string event_name = "Res::" + Message_Code_Name(message.code());
    ApiEventFactory factory = ApiHandlerRegistry::Get(event_name);
    EventRef event = factory(message, this->drain);

    // Initialise event and return.
    EventLifecycle::Init(event);
    return event;
  }
};


void Client::connectToServer() {
  std::string path = "/tmp/snow-fox.socket";
  std::string drain_id = ApiClient::Connect(path);
  sf::core::context::Client::server(drain_id);
}

void Client::introduceToServer() {
  int server = sf::core::context::Client::server()->fd();
  Message introduce;
  introduce.set_code(Message::Introduce);
  MessageIO<Message>::send(server, introduce);
}


void Client::initialise() {
  // Initialise event sources.
  this->registerDefaultSourceManager();
  Context::LoopManager()->add(EventSourceRef(new ManualSource()));

  // Initialise client.
  this->maskSignals();
  this->connectToServer();
  ClientLifecycle::Lua::Init();
  this->introduceToServer();
}
