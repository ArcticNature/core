// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_LIFECYCLE_CONFIG_NODE_H_
#define CORE_LIFECYCLE_CONFIG_NODE_H_

#include "core/interface/lifecycle.h"
#include "core/utility/lua.h"


// Forward declare needed config interfaces.
namespace sf {
namespace core {
namespace interface {

  class NodeConfigLoader;

}  // namespace interface
}  // namespace core
}  // namespace sf


namespace sf {
namespace core {
namespace lifecycle {

  //! Aliases to node's configuration stages.
  class NodeConfigLifecycle {
   public:
    static void InitLua(
        sf::core::interface::NodeConfigLoader* loader,
        sf::core::utility::Lua* lua
    );
  };

  //! Argument to config::node::* lifecycle events.
  class NodeConfigLifecycleArg : public sf::core::interface::BaseLifecycleArg {
   protected:
    sf::core::utility::Lua* _lua;
    sf::core::interface::NodeConfigLoader* _loader;

   public:
    NodeConfigLifecycleArg(
        sf::core::interface::NodeConfigLoader* loader,
        sf::core::utility::Lua* lua
    );

    //! The node configuration loader instance driving the process.
    sf::core::interface::NodeConfigLoader* loader() const;

    //! The Lua environment used to load and run the config.
    sf::core::utility::Lua* lua() const;
  };

}  // namespace lifecycle
}  // namespace core
}  // namespace sf

#endif  // CORE_LIFECYCLE_CONFIG_NODE_H_
