// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/cluster/node.h"
#include "core/context/static.h"

using sf::core::cluster::Node;
using sf::core::context::Static;
using sf::core::model::CLIParser;


class NullParser : public CLIParser {
 protected:
  void parseLogic(int* argc, char*** argv) {
    return;
  }
};


class NodeTest : public ::testing::Test {
 public:
  NodeTest() {
    Static::parser(new NullParser());
  };

  ~NodeTest() {
    Node::reset();
    Static::destroy();
  };
};


TEST_F(NodeTest, DefaultName) {
  ASSERT_EQ("node", Node::name());
}

TEST_F(NodeTest, NameFromCli) {
  Static::parser()->setString("node-name", "test-node");
  ASSERT_EQ("test-node", Node::name());
}
