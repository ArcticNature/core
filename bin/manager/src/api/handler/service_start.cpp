// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <memory>
#include <string>

#include "core/config/service.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_service.pb.h"
#include "core/registry/event/handler/api.h"
#include "core/service/description.h"


using sf::core::config::ServiceLoader;
using sf::core::context::Static;
using sf::core::context::ProxyLogger;

using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::LogInfo;

using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::ServiceStart;
using sf::core::registry::ApiHandlerRegistry;
using sf::core::service::ServiceDescription;


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

    // Fetch service definition from repo.
    auto repo = Static::repository()->lookup(this->version_);
    auto desc = repo->findDefinitionFile(this->service_id_, "service");
    auto blob = repo->readFile(desc);

    // Load instance description.
    // TODO(stefano): use exact version instead of symbolic one.
    auto loader = std::make_shared<ServiceLoader>(blob, this->version_);
    loader->load().then(
      [&, this](ServiceDescription desc) {  // NOLINT(build/c++11)
        // TODO(stefano): Add starting instance into the register.
        // TODO(stefano): Send instance start to spawner.

        return nullptr;
      }
    );
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
