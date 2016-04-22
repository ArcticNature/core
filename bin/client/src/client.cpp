// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/client.h"

#include <string>

#include "core/event/client/unix.h"
#include "core/event/drain/fd.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/utility/protobuf.h"

using sf::core::bin::Client;

using sf::core::event::FdDrain;
using sf::core::event::UnixClient;
using sf::core::model::EventRef;

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
  this->registerDefaultSourceManager();
  this->maskSignals();
  this->connectToServer();
}
