// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/cluster/cluster.h"

#include "core/exceptions/base.h"


using sf::core::cluster::ClusterRaw;
using sf::core::exception::ContextUninitialised;
using sf::core::interface::MetaDataStoreRef;


ClusterRaw::ClusterRaw(
    MetaDataStoreRef metadata
) {
  this->metadata_ = metadata;
}

MetaDataStoreRef ClusterRaw::metadata() const {
  if (!this->metadata_) {
    throw ContextUninitialised("No metadata given to cluster");
  }
  return this->metadata_;
}
