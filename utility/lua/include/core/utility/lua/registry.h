// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_REGISTRY_H_
#define CORE_UTILITY_LUA_REGISTRY_H_

#include <string>

#include "core/utility/lua.h"

namespace sf {
namespace core {
namespace utility {

  //! Wrapper class for lua tables.
  class LuaRegistry {
   protected:
    Lua* state;

   public:
    //! Wrap the registry for the given state.
    explicit LuaRegistry(Lua* state);

    //! Store a light user data (a pointer) into the registry.
    void store(std::string key, void* lightUserData);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_REGISTRY_H_

