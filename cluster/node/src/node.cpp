// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/cluster/node.h"

#include <memory>
#include <string>
#include <vector>

#include "core/model/repository.h"
#include "core/utility/status.h"


using sf::core::cluster::NodeRaw;
using sf::core::cluster::NodeVersion;
using sf::core::model::RepositoryVersionId;

using sf::core::utility::SubsystemStatus;
using sf::core::utility::Status;
using sf::core::utility::StatusLight;


std::vector<std::string> KNOWN_SUBSYSTEMS = {
  // "cluster",
  "configuration",
  "node"
};


NodeRaw::NodeRaw(std::string name, NodeVersion version) :
    name_(name), version_(version),
    config_version_("", "<system-starting>") {
  // Register known subsystems to UNKOWN state.
  for (auto subsys : KNOWN_SUBSYSTEMS) {
    this->status_.set(subsys, Status(
      StatusLight::UNKOWN, "System initialising"
    ));
  }
}

RepositoryVersionId NodeRaw::configVersion() const {
  return this->config_version_;
}

void NodeRaw::configVersion(RepositoryVersionId ver) {
  this->config_version_ = ver;
}

std::string NodeRaw::name() const {
  return this->name_;
}

SubsystemStatus* NodeRaw::status() {
  return &this->status_;
}

NodeVersion NodeRaw::version() const {
  return this->version_;
}
