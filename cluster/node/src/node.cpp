// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/cluster/node.h"

#include <memory>
#include <string>
#include <vector>

#include "core/compile-time/options.h"
#include "core/compile-time/version.h"
#include "core/context/static.h"

using sf::core::cluster::Node;
using sf::core::cluster::NodeStatus;
using sf::core::cluster::NodeStatusCode;
using sf::core::cluster::NodeStatusDetail;
using sf::core::cluster::NodeVersion;

using sf::core::context::Static;
using sf::core::model::RepositoryVersionId;
using sf::core::utility::StatusDetail;


std::vector<std::string> KNOWN_SUBSYSTEMS = {
  "configuration",
  "process"
};


NodeStatusDetail::NodeStatusDetail(
    NodeStatusCode code, std::string message
) : StatusDetail<
    NodeStatusCode, NodeStatusCode::OK,
    NodeStatusCode::WARNING, NodeStatusCode::ERROR,
    NodeStatusCode::END
>(code, message) {
}


NodeStatus::NodeStatus() {
  // Register known subsystems to UNKOWN state.
  for (std::vector<std::string>::iterator it = KNOWN_SUBSYSTEMS.begin();
      it != KNOWN_SUBSYSTEMS.end(); it++) {
    this->set(*it, NodeStatusDetail(
        NodeStatusCode::UNKOWN, "System initialising"
    ));
  }
}


std::shared_ptr<Node> Node::_me;


Node* Node::me() {
  // Return singleton instance if possible.
  if (Node::_me) {
    return Node::_me.get();
  }

  // Build name.
  std::string name = "node";
  if (Static::parser()->hasString("node-name")) {
    name = Static::parser()->getString("node-name");
  }

  // Build version.
  NodeVersion version = {
    VERSION_SHA, VERSION_TAINT, VERSION_NUMBER
  };

  // Create instance.
  Node::_me = std::shared_ptr<Node>(new Node(name, version));
  return Node::_me.get();
}

#if TEST_BUILD
void Node::reset() {
  Node::_me = std::shared_ptr<Node>();
}
#endif


Node::Node(std::string name, NodeVersion version) :
    _name(name), _version(version), config_version("", "<system-starting>") {
  //
}

RepositoryVersionId Node::configVersion() const {
  return this->config_version;
}

void Node::configVersion(RepositoryVersionId ver) {
  this->config_version = ver;
}

std::string Node::name() const {
  return this->_name;
}

NodeStatus* Node::status() {
  return &this->_status;
}

NodeVersion Node::version() const {
  return this->_version;
}
