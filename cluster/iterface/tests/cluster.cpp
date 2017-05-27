// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/cluster/cluster.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"

#include "core/testing/metadata.h"
#include "core/testing/static.h"


using sf::core::cluster::Cluster;
using sf::core::cluster::ClusterRaw;
using sf::core::cluster::Node;

using sf::core::context::Static;
using sf::core::exception::ContextUninitialised;
using sf::core::interface::MetaDataStoreRef;

using sf::core::testing::StaticContextTest;
using sf::core::testing::TestMetaStore;


class ClusterTest : public StaticContextTest {
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
    Cluster::Destroy();
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

TEST_F(ClusterTest, MyselfSingleton) {
  Node me1 = this->cluster_->myself();
  Node me2 = this->cluster_->myself();
  ASSERT_NE(nullptr, me1.get());
  ASSERT_EQ(me1, me2);
}

TEST_F(ClusterTest, MyselfDefaultName) {
  std::string hostname;
  {
    char buffer[1024];
    int code = gethostname(buffer, 1024);
    ASSERT_EQ(0, code);
    hostname = std::string(buffer);
  }

  Node me = this->cluster_->myself();
  ASSERT_EQ(hostname, me->name());
}

TEST_F(ClusterTest, MyselfNameFromCLI) {
  Static::parser()->setString("node-name", "test-node-name");
  Node me = this->cluster_->myself();
  ASSERT_EQ("test-node-name", me->name());
}

TEST_F(ClusterTest, UninitialisedInstance) {
  ASSERT_THROW(Cluster::Instance(), ContextUninitialised);
}

TEST_F(ClusterTest, SetInstance) {
  Cluster::Instance(this->cluster_);
  ASSERT_EQ(this->cluster_, Cluster::Instance());
}
