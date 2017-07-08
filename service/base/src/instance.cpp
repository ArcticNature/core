// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/service/instance.h"

#include <json.hpp>
#include <string>

#include "core/service/description.h"
#include "core/utility/random.h"


using nlohmann::json;

using sf::core::service::ServiceDescription;
using sf::core::service::ServiceInstance;
using sf::core::utility::IdentifierGenerator;


ServiceInstance::ServiceInstance(ServiceDescription description) :
    description_(description) {
  IdentifierGenerator generator;
  this->global_instance_id_ = generator.generate(2);
  this->state_ = ServiceInstance::State::UNALLOCATED;
}

ServiceDescription ServiceInstance::description() const {
  return this->description_;
}

std::string ServiceInstance::globalId() const {
  return this->global_instance_id_;
}

json ServiceInstance::jsonify() const {
  json desc = json::object();
  desc["__type"] = "ServiceInstance";
  desc["__version"] = 1;
  desc["description"] = {
    {"hash", this->description_.hash()},
    {"service_id", this->description_.serviceId()},
    {"config_version", this->description_.configVersion()}
  };
  desc["global_instance_id"] = this->global_instance_id_;
  desc["node"]["instance_id"] = this->node_instance_id_;
  desc["node"]["name"] = this->node_name_;
  desc["state"] = this->state_;
  return desc;
}
