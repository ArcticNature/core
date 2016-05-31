// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager/api/source.h"

#include <string>

#include "core/event/source/fd.h"
#include "core/exceptions/base.h"
#include "core/lifecycle/event.h"

#include "core/model/event.h"
#include "core/protocols/public/p_message.pb.h"
#include "core/registry/event/handler/api.h"
#include "core/utility/protobuf.h"

using sf::core::bin::ApiFdSource;
using sf::core::event::FdSource;
using sf::core::exception::CorruptedData;
using sf::core::lifecycle::EventLifecycle;

using sf::core::model::EventRef;
using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::Message_Code_Name;

using sf::core::registry::ApiEventFactory;
using sf::core::registry::ApiHandlerRegistry;
using sf::core::utility::MessageIO;


ApiFdSource::ApiFdSource(
    int fd, std::string id, std::string drain
) : FdSource(fd, id, drain) {
  // NOOP.
}

EventRef ApiFdSource::parse() {
  if (!this->checkFD()) {
    return EventRef();
  }

  // Parse event.
  int fd = this->getFD();
  Message message;
  bool parsed = MessageIO<Message>::parse(fd, &message);
  if (!parsed) {
    throw CorruptedData("Unable to parse protocol buffer");
  }

  // Create event.
  std::string event_name = "Req::" + Message_Code_Name(message.code());
  ApiEventFactory factory = ApiHandlerRegistry::Get(event_name);
  EventRef event = factory(message, this->drain_id);

  // Initialise event and return.
  EventLifecycle::Init(event);
  return event;
}
