// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CLUSTER_NODE_H_
#define CORE_CLUSTER_NODE_H_

#include <memory>
#include <string>

#include "core/compile-time/options.h"

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

   public:
    Node(std::string name, NodeVersion version);

    //! Returns the name of the node.
    std::string name() const;
  };

}  // namespace cluster
}  // namespace core
}  // namespace sf

#endif  // CORE_CLUSTER_NODE_H_
