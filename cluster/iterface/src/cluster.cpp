// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/cluster/cluster.h"

#include <string>

#include "core/compile-time/options.h"
#include "core/compile-time/version.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"


using sf::core::cluster::Cluster;
using sf::core::cluster::ClusterRaw;
using sf::core::cluster::Node;
using sf::core::cluster::NodeRaw;

using sf::core::context::Static;
using sf::core::exception::ContextUninitialised;
using sf::core::interface::MetaDataStoreRef;


// Store the active cluster instance and Node self.
static Cluster cluster_instance;
static Node myself_instance;


Cluster Cluster::Instance() {
  if (!cluster_instance) {
    throw ContextUninitialised("Cluster instance not initialised");
  }
  return cluster_instance;
}

void Cluster::Instance(Cluster instance) {
  cluster_instance = instance;
}

void Cluster::Destroy() {
  cluster_instance.reset();
  myself_instance.reset();
}


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

Node ClusterRaw::myself() {
  // If the cached instance is missing create it.
  if (!myself_instance) {
    // Build node name (from CLI or hostname as a fallback).
    std::string name = "<unkown>";
    if (Static::parser()->hasString("node-name")) {
      name = Static::parser()->getString("node-name");
    } else {
      char buffer[HOSTNAME_BUFFER_LEN];
      Static::posix()->gethostname(buffer, HOSTNAME_BUFFER_LEN);
      name = std::string(buffer);
    }

    // Build version.
    NodeVersion version{
      VERSION_SHA, VERSION_TAINT, VERSION_NUMBER
    };

    // Create instance.
    myself_instance = std::make_shared<NodeRaw>(name, version);
  }
  return myself_instance;
}
