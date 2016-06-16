// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/cluster/node.h"
#include "core/context/static.h"
#include "core/model/repository.h"

using sf::core::cluster::Node;
using sf::core::cluster::NodeStatusCode;
using sf::core::context::Static;

using sf::core::model::CLIParser;
using sf::core::model::RepositoryVersionId;


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
  ASSERT_EQ("node", Node::me()->name());
}

TEST_F(NodeTest, NameFromCli) {
  Static::parser()->setString("node-name", "test-node");
  ASSERT_EQ("test-node", Node::me()->name());
}

TEST_F(NodeTest, SingletonMe) {
  Node* node1 = Node::me();
  Node* node2 = Node::me();
  ASSERT_EQ(node1, node2);
}

TEST_F(NodeTest, Status) {
  ASSERT_EQ(
      NodeStatusCode::UNKOWN,
      Node::me()->status()->reason().code()
  );
}

TEST_F(NodeTest, DefaultConfigVersion) {
  RepositoryVersionId version = Node::me()->configVersion();
  ASSERT_EQ("<system-starting>", version.symbolic());
  ASSERT_EQ("", version.effective());
}

TEST_F(NodeTest, UpdateConfigVersion) {
  RepositoryVersionId version("a", "b");
  Node::me()->configVersion(version);
  version = Node::me()->configVersion();
  ASSERT_EQ("b", version.symbolic());
  ASSERT_EQ("a", version.effective());
}
