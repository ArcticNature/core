// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_INTERFACE_CONFIG_NODE_HOOKS_H_
#define CORE_INTERFACE_CONFIG_NODE_HOOKS_H_

#include "core/hook.h"
#include "core/interface/config/node.h"
#include "core/utility/lua.h"


namespace sf {
namespace core {
namespace hook {

  //! Node configuration extention hooks.
  class NodeConfig {
   public:
    //! Triggered when a new config environment is created.
    /*!
     * Usefull to create global variables, extend existing
     * choices, and the like.
     *
     * DO NOT rely on the order of initialisation as that
     * is not predictable.
     * Make use of lazy initialisation if you need to depend
     * on something that is not in the enviroment yet.
     */
    static Hook<sf::core::utility::Lua> LuaInit;

    //! Triggered to collect node configuration directives.
    /*!
     * Configuration is driven by intents that are manipulated
     * in the lua environment.
     *
     * Once the environment is configured as desired, intents
     * are collected, sorted, and executed to verify and perform
     * configuration.
     *
     * This hook is invoked to collect extention provided intents.
     */
    static Hook<
      sf::core::utility::Lua,
      sf::core::interface::NodeConfigLoader*
    > Collect;
  };

}  // namespace hook
}  // namespace core
}  // namespace sf

#endif  // CORE_INTERFACE_CONFIG_NODE_HOOKS_H_
