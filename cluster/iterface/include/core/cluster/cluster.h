// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CLUSTER_CLUSTER_H_
#define CORE_CLUSTER_CLUSTER_H_

#include <memory>

#include "core/cluster/node.h"
#include "core/interface/metadata/store.h"


namespace sf {
namespace core {
namespace cluster {

  // Forward declare.
  class ClusterRaw;


  //! Extended shared_ptr for nicer code.
  class Cluster : public std::shared_ptr<ClusterRaw> {
   public:
    static Cluster Instance();
    static void Instance(Cluster instance);
    static void Destroy();

   public:
    // Inherit the base constructors.
    using std::shared_ptr<ClusterRaw>::shared_ptr;
  };


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

    //! Returns a node instance representing the local node.
    Node myself();
  };

}  // namespace cluster
}  // namespace core
}  // namespace sf

#endif  // CORE_CLUSTER_CLUSTER_H_
