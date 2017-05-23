// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CLUSTER_CLUSTER_H_
#define CORE_CLUSTER_CLUSTER_H_

#include <memory>

#include "core/interface/metadata/store.h"


namespace sf {
namespace core {
namespace cluster {

  //! Interface to cluster operations and attributes.
  class ClusterRaw : public std::enable_shared_from_this<ClusterRaw> {
   protected:
    sf::core::interface::MetaDataStoreRef metadata_;

   public:
    //! Create a new cluster interface with the given attributes.
    ClusterRaw(
        sf::core::interface::MetaDataStoreRef metadata
    );

    //! Returns the cluster metadata store.
    sf::core::interface::MetaDataStoreRef metadata() const;
  };
  typedef std::shared_ptr<ClusterRaw> Cluster;

}  // namespace cluster
}  // namespace core
}  // namespace sf

#endif  // CORE_CLUSTER_CLUSTER_H_
