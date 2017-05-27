// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/testing/cluster.h"

#include "core/cluster/cluster.h"
#include "core/testing/metadata.h"


using sf::core::cluster::Cluster;
using sf::core::cluster::ClusterRaw;

using sf::core::testing::ClusterTest;
using sf::core::testing::TestMetaStore;


ClusterTest::ClusterTest() {
  Cluster cluster = std::make_shared<ClusterRaw>(
      std::make_shared<TestMetaStore>()
  );
  Cluster::Instance(cluster);
}

ClusterTest::~ClusterTest() {
  Cluster::Destroy();
}
