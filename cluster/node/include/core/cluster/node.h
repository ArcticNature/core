// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CLUSTER_NODE_H_
#define CORE_CLUSTER_NODE_H_

#include <memory>
#include <string>

#include "core/model/repository.h"
#include "core/utility/status.h"

namespace sf {
namespace core {
namespace cluster {

  //! Version metadata for a node.
  struct NodeVersion {
    std::string commit;
    std::string taint;
    std::string version;
  };

  //! Node metadata.
  class NodeRaw {
   protected:
    const std::string name_;
    const NodeVersion version_;

    sf::core::utility::SubsystemStatus status_;
    sf::core::model::RepositoryVersionId config_version_;

   public:
    NodeRaw(std::string name, NodeVersion version);

    //! Returns or sets the current configuration version.
    sf::core::model::RepositoryVersionId configVersion() const;
    void configVersion(sf::core::model::RepositoryVersionId ver);

    //! Returns the name of the node.
    std::string name() const;

    //! Returns the node status traker.
    sf::core::utility::SubsystemStatus* status();

    //! Returns the node version information.
    NodeVersion version() const;
  };
  typedef std::shared_ptr<NodeRaw> Node;

}  // namespace cluster
}  // namespace core
}  // namespace sf

#endif  // CORE_CLUSTER_NODE_H_
