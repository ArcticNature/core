// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/cluster/node.h"

#include <string>

#include "core/compile-time/options.h"
#include "core/context/static.h"

using sf::core::cluster::Node;
using sf::core::context::Static;


std::string Node::_name("");


std::string Node::name() {
  if (Node::_name == "") {
    Node::_name = "node";
    if (Static::parser()->hasString("node-name")) {
      Node::_name = Static::parser()->getString("node-name");
    }
  }
  return Node::_name;
}


#if TEST_BUILD
void Node::reset() {
  Node::_name = "";
}
#endif
