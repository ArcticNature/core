// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <memory>
#include <string>
#include <vector>

#include <json.hpp>

#include "core/cluster/cluster.h"
#include "core/config/service.h"

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/protocols/manager_spawner/ms_message.pb.h"
#include "core/protocols/manager_spawner/ms_process.pb.h"
#include "core/protocols/public/p_message.pb.h"
#include "core/protocols/public/p_service.pb.h"

#include "core/registry/event/handler/api.h"
#include "core/utility/protobuf.h"

#include "core/service/description.h"
#include "core/service/instance.h"


using nlohmann::json;

using SpawnerMessage = sf::core::protocol::manager_spawner::Message;

using sf::core::cluster::Cluster;
using sf::core::config::ServiceLoader;

using sf::core::context::Static;
using sf::core::context::ProxyLogger;

using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::LogInfo;

using sf::core::protocol::manager_spawner::StartProcess;
using sf::core::protocol::public_api::Message;
using sf::core::protocol::public_api::ServiceStart;

using sf::core::registry::ApiHandlerRegistry;
using sf::core::utility::MessageIO;

using sf::core::service::ServiceDescription;
using sf::core::service::ServiceInstance;


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
        // Add starting instance into the register.
        ServiceInstance instance(desc);
        return Cluster::Instance()->metadata()->set(
          instance.globalId(), instance.jsonify()
        ).then([instance]() {
          return instance;
        });
      }

    ).then([&, this](ServiceInstance instance) {  // NOLINT(build/c++11)
        LogInfo log = {
          {"instance_id", instance.globalId()},
          {"service", this->service_id_},
        };
        INFOV(
            logger,
            "Registered instance '${instance_id}' of '${service}'.", log
        );

        // Create protobuf message.
        SpawnerMessage message;
        message.set_code(SpawnerMessage::StartProcess);
        message.set_correlation_id(this->correlation());
        StartProcess* start = message.MutableExtension(StartProcess::msg);

        // Populate start request.
        ServiceDescription desc = instance.description();
        for (auto pair : desc.environment()) {
          auto* item = start->add_environment();
          item->set_name(pair.first);
          item->set_value(pair.second);
        }
        start->set_group_name(desc.group());
        start->set_user_name(desc.user());

        json connector = desc.connectorConfig();
        std::vector<std::string> command = connector["command"];
        for (auto arg : command) {
          start->add_command(arg);
        }
        if (connector["workdir"].is_string()) {
          start->set_work_dir(connector["workdir"]);
        }

        // Send instance start to spawner.
        auto spawner_id = Static::options()->getString("spawner-drain-id");
        auto spawner = Static::drains()->get(spawner_id);
        MessageIO<SpawnerMessage>::send(spawner, message);
        INFOV(logger, "Sent instance '${instance_id}' start.", log);
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
