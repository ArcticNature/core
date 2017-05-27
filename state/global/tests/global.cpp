// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/state/global.h"

#include "core/testing/cluster.h"
#include "core/testing/metadata.h"


using sf::core::cluster::Cluster;
using sf::core::cluster::ClusterRaw;
using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::exception::ContextUninitialised;

using sf::core::state::GlobalState;
using sf::core::testing::ClusterTest;
using sf::core::testing::TestMetaStore;


TEST_F(ClusterTest, Capture) {
  GlobalState global = GlobalState::Capture();
  ASSERT_EQ(Cluster::Instance(), global.cluster);
  ASSERT_EQ(Context::Instance(), global.context);
}

TEST_F(ClusterTest, Destroy) {
  ContextRef context = Context::Instance();
  ASSERT_NO_THROW(Cluster::Instance());
  GlobalState::Destroy();
  ASSERT_THROW(Cluster::Instance(), ContextUninitialised);
  ASSERT_NE(context, Context::Instance());
}

TEST_F(ClusterTest, Restore) {
  GlobalState global = GlobalState::Capture();
  Cluster::Instance(std::make_shared<ClusterRaw>(
      std::make_shared<TestMetaStore>()
  ));
  Context::Initialise(std::make_shared<Context>());
  ASSERT_NE(global.cluster, Cluster::Instance());
  ASSERT_NE(global.context, Context::Instance());

  global.restore();
  ASSERT_EQ(global.cluster, Cluster::Instance());
  ASSERT_EQ(global.context, Context::Instance());
}
