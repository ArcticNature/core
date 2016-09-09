// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CLUSTER_NODE_H_
#define CORE_CLUSTER_NODE_H_

#include <memory>
#include <string>

#include "core/compile-time/options.h"
#include "core/model/repository.h"
#include "core/utility/status.h"

namespace sf {
namespace core {
namespace cluster {

  //! Valid system status codes.
  enum NodeStatusCode {
    UNKOWN = -1,
    OK,
    CONFIG_OK,
    PROCESS_READY,
    WARNING,
    CONFIG_LOAD,
    PROCESS_STARTING,
    ERROR,
    END
  };


  //! Represents a node's status details.
  class NodeStatusDetail : public sf::core::utility::StatusDetail<
      NodeStatusCode, NodeStatusCode::OK,
      NodeStatusCode::WARNING, NodeStatusCode::ERROR,
      NodeStatusCode::END
  > {
   public:
    NodeStatusDetail(NodeStatusCode code, std::string message);
  };


  //! Represents a node's status.
  class NodeStatus : public sf::core::utility::SubsystemStatus<
      NodeStatusDetail, NodeStatusCode
  > {
   public:
    NodeStatus();
  };


  //! Version metadata for a node.
  struct NodeVersion {
    std::string commit;
    std::string taint;
    std::string version;
  };

  //! Node metadata.
  class Node {
   protected:
    static std::shared_ptr<Node> _me;

   public:
    static Node* me();

#if TEST_BUILD
    static void reset();
#endif

   protected:
    const std::string _name;
    const NodeVersion _version;

    NodeStatus _status;
    sf::core::model::RepositoryVersionId config_version;

   public:
    Node(std::string name, NodeVersion version);

    //! Returns or sets the current configuration version.
    sf::core::model::RepositoryVersionId configVersion() const;
    void configVersion(sf::core::model::RepositoryVersionId ver);

    //! Returns the name of the node.
    std::string name() const;

    //! Returns the node status traker.
    NodeStatus* status();

    //! Returns the node version information.
    NodeVersion version() const;
  };

}  // namespace cluster
}  // namespace core
}  // namespace sf

#endif  // CORE_CLUSTER_NODE_H_
