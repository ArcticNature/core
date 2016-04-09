// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/cluster/node.h"

#include <memory>
#include <string>

#include "core/compile-time/options.h"
#include "core/compile-time/version.h"
#include "core/context/static.h"

using sf::core::cluster::Node;
using sf::core::cluster::NodeVersion;
using sf::core::context::Static;


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
    _name(name), _version(version) {
  //
}

std::string Node::name() const {
  return this->_name;
}
