// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONFIG_HOOKS_H_
#define CORE_CONFIG_HOOKS_H_


#include "core/hook.h"
#include "core/config/base.h"
#include "core/utility/lua.h"


namespace sf {
namespace core {
namespace hook {

  //! Base configuration loader hooks.
  class ConfigLoader {
   public:
    static Hook<sf::core::config::ConfigLoader, sf::core::utility::Lua> \
      InitialiseLua;
  };

}  // namespace hook
}  // namespace core
}  // namespace sf

#endif  // CORE_CONFIG_HOOKS_H_
