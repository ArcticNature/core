// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <memory>

#include "core/context/context.h"
#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_service.pb.h"
#include "core/registry/event/handler/api.h"


using sf::core::context::ProxyLogger;

using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::LogInfo;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::ServiceStart;
using sf::core::registry::ApiHandlerRegistry;


static ProxyLogger logger("core.bin.manager.api.handler.service_start");


class ServiceStartEvent : public Event {
 protected:
  std::string service_id_;
  std::string version_;

 public:
  ServiceStartEvent(
      std::string correlation, EventDrainRef drain,
      std::string service_id, std::string version
  ) : Event(correlation, drain) {
    this->service_id_ = service_id;
    this->version_ = version;
  }

  void handle() {
    LogInfo log = {
      {"service_id", this->service_id_},
      {"version", this->version_}
    };
    INFOV(
        logger,
        "Recieved request to start an instance of '${service_id}' "
        "at version '${version}'",
        log
    );

    // TODO(stefano): Load instance description.
    // TODO(stefano): Add starting instance into the register.
    // TODO(stefano): Send instance start to spawner.
  }
};


//! Event factory for ServiceStart requests.
EventRef service_start(Message message, EventDrainRef drain) {
  const ServiceStart& service = message.GetExtension(ServiceStart::msg);
  return std::make_shared<ServiceStartEvent>(
      message.correlation_id(), drain,
      service.service_id(), service.version()
  );
}

// Register event factory.
StaticFactory(ApiHandlerRegistry, "Req::ServiceStart", service_start);
