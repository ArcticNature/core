// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CLUSTER_NODE_H_
#define CORE_CLUSTER_NODE_H_

#include <string>

#include "core/compile-time/options.h"

namespace sf {
namespace core {
namespace cluster {

  //! Node metadata.
  class Node {
   protected:
    static std::string _name;

   public:
    //! Returns the name of the node.
    static std::string name();

#if TEST_BUILD
    static void reset();
#endif
  };

}  // namespace cluster
}  // namespace core
}  // namespace sf

#endif  // CORE_CLUSTER_NODE_H_
