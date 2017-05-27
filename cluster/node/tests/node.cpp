// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/cluster/node.h"
#include "core/model/repository.h"

using sf::core::cluster::Node;
using sf::core::cluster::NodeRaw;
using sf::core::cluster::NodeVersion;

using sf::core::model::RepositoryVersionId;
using sf::core::utility::StatusLight;
using sf::core::utility::SubsystemStatus;


TEST(Node, Name) {
  NodeVersion version;
  Node node = std::make_shared<NodeRaw>("node", version);
  ASSERT_EQ("node", node->name());
}

TEST(Node, Status) {
  NodeVersion version;
  Node node = std::make_shared<NodeRaw>("node", version);
  SubsystemStatus* status = node->status();

  ASSERT_EQ(StatusLight::UNKOWN, status->colour());
  ASSERT_EQ("System initialising", status->message());
}

TEST(Node, DefaultConfigVersion) {
  NodeVersion version;
  Node node = std::make_shared<NodeRaw>("node", version);
  RepositoryVersionId config = node->configVersion();
  ASSERT_EQ("<system-starting>", config.symbolic());
  ASSERT_EQ("", config.effective());
}

TEST(Node, UpdateConfigVersion) {
  NodeVersion version;
  Node node = std::make_shared<NodeRaw>("node", version);
  RepositoryVersionId config("a", "b");
  node->configVersion(config);
  config = node->configVersion();
  ASSERT_EQ("b", config.symbolic());
  ASSERT_EQ("a", config.effective());
}

TEST(Node, Version) {
  NodeVersion version{"a", "b", "c"};
  Node node = std::make_shared<NodeRaw>("node", version);

  NodeVersion actual = node->version();
  ASSERT_EQ(version.commit, actual.commit);
  ASSERT_EQ(version.taint, actual.taint);
  ASSERT_EQ(version.version, actual.version);
}
