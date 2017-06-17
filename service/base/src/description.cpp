// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/service/description.h"

#include <climits>
#include <map>
#include <string>

#include "core/exceptions/base.h"
#include "core/exceptions/configuration.h"
#include "core/exceptions/options.h"


using nlohmann::json;

using sf::core::exception::InvalidOption;
using sf::core::exception::MissingConfiguration;
using sf::core::exception::TypeError;

using sf::core::service::ServiceDescription;
using sf::core::service::ServiceDescriptionBuilder;


ServiceDescriptionBuilder ServiceDescription::Build() {
  ServiceDescriptionBuilder builder;
  return builder;
}

ServiceDescription ServiceDescription::Load(json object) {
  if (object["__type"] != "ServiceDescription" || object["__version"] != 1) {
    throw TypeError("Invalid json object for ServiceDescription::Load");
  }

  ServiceDescription desc;
  desc.config_version_ = object["config_version"];
  desc.connector_ = object["connector"]["name"];
  desc.connector_config_ = object["connector"]["config"];
  desc.service_id_ = object["service_id"];

  for (auto pair : json::iterator_wrapper(object["environment"])) {
    desc.environment_[pair.key()] = pair.value();
  }

  for (auto pair : json::iterator_wrapper(object["ports"])) {
    desc.ports_[pair.key()] = pair.value();
  }

  desc.group_ = object["group"];
  desc.user_ = object["user"];
  return desc;
}

std::size_t ServiceDescription::hash() const {
  std::string payload = this->service_id_ + "<:::>" + this->config_version_;
  return std::hash<std::string>()(payload);
}

json ServiceDescription::jsonify() const {
  json desc = json::object();
  desc["__type"] = "ServiceDescription";
  desc["__version"] = 1;
  desc["config_version"] = this->config_version_;
  desc["service_id"] = this->service_id_;

  desc["connector"] = json{
    {"name", this->connector_},
    {"config", this->connector_config_}
  };

  desc["environment"] = this->environment_;
  desc["ports"] = this->ports_;

  desc["group"] = this->group_;
  desc["user"] = this->user_;
  return desc;
}

std::string ServiceDescription::configVersion() const {
  return this->config_version_;
}

std::string ServiceDescription::connector() const {
  return this->connector_;
}

json ServiceDescription::connectorConfig() const {
  return this->connector_config_;
}

std::map<std::string, std::string> ServiceDescription::environment() const {
  return this->environment_;
}

std::string ServiceDescription::group() const {
  return this->group_;
}

uint32_t ServiceDescription::port(std::string name) const {
  // TODO(stefano): throw if the port is not set.
  return this->ports_.at(name);
}

std::map<std::string, uint32_t> ServiceDescription::ports() const {
  return this->ports_;
}

std::string ServiceDescription::serviceId() const {
  return this->service_id_;
}

std::string ServiceDescription::user() const {
  return this->user_;
}


ServiceDescription ServiceDescriptionBuilder::build() {
  if (this->description_.service_id_ == "") {
    throw MissingConfiguration("A ServiceDesctiption needs a service id");
  }
  if (this->description_.config_version_ == "") {
    throw MissingConfiguration("A ServiceDesctiption needs a config version");
  }
  if (this->description_.connector_ == "") {
    throw MissingConfiguration("A ServiceDesctiption needs a connector");
  }
  return this->description_;
}

ServiceDescriptionBuilder ServiceDescriptionBuilder::configVersion(
    std::string config_version
) {
  this->description_.config_version_ = config_version;
  return *this;
}

ServiceDescriptionBuilder ServiceDescriptionBuilder::connector(
    std::string connector
) {
  this->description_.connector_ = connector;
  return *this;
}

ServiceDescriptionBuilder ServiceDescriptionBuilder::connectorConfig(
    json config
) {
  this->description_.connector_config_ = config;
  return *this;
}

ServiceDescriptionBuilder ServiceDescriptionBuilder::environment(
    std::string key, std::string value
) {
  this->description_.environment_[key] = value;
  return *this;
}

ServiceDescriptionBuilder ServiceDescriptionBuilder::group(
    std::string group
) {
  this->description_.group_ = group;
  return *this;
}

ServiceDescriptionBuilder ServiceDescriptionBuilder::port(
    std::string name, uint32_t port
) {
  if (port <= 0 || port > USHRT_MAX) {
    throw InvalidOption(
        "Cannot allocate port outside the [0, " +
        std::to_string(USHRT_MAX) + "] range"
    );
  }
  if (this->used_ports_.find(port) != this->used_ports_.end()) {
    throw InvalidOption("Cannot allocate the same port twice");
  }
  this->used_ports_.insert(port);
  this->description_.ports_[name] = port;
  return *this;
}

ServiceDescriptionBuilder ServiceDescriptionBuilder::serviceId(
    std::string service_id
) {
  this->description_.service_id_ = service_id;
  return *this;
}

ServiceDescriptionBuilder ServiceDescriptionBuilder::user(
    std::string user
) {
  this->description_.user_ = user;
  return *this;
}
