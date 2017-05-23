// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/cluster/cluster.h"
#include "core/exceptions/base.h"

#include "core/testing/metadata.h"


using sf::core::cluster::Cluster;
using sf::core::cluster::ClusterRaw;

using sf::core::exception::ContextUninitialised;
using sf::core::interface::MetaDataStoreRef;

using sf::core::testing::TestMetaStore;


class ClusterTest : public ::testing::Test {
 protected:
  MetaDataStoreRef metadata_;
  Cluster cluster_;

 public:
  ClusterTest() {
    this->metadata_ = std::make_shared<TestMetaStore>();
    this->cluster_ = std::make_shared<ClusterRaw>(this->metadata_);
  }

  ~ClusterTest() {
    this->cluster_.reset();
    this->metadata_.reset();
  }
};


TEST_F(ClusterTest, MetadataIsReturned) {
  MetaDataStoreRef meta = this->cluster_->metadata();
  ASSERT_EQ(this->metadata_, meta);
}

TEST_F(ClusterTest, MetadataMustBeSet) {
  Cluster cluster = std::make_shared<ClusterRaw>(MetaDataStoreRef());
  ASSERT_THROW(cluster->metadata(), ContextUninitialised);
}
