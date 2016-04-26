// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_REGISTRY_H_
#define CORE_UTILITY_LUA_REGISTRY_H_

#include <string>

#include "lua.hpp"
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

    //! Pushes the referenced value onto the stack.
    void dereference(lua_Integer ref);

    //! Frees a reference allocated with `referenceTop`.
    void freeReference(lua_Integer ref);

    //! Returns a reference to the top of the stack and pops it.
    lua_Integer referenceTop();

    //! Store a light userdata (a pointer) into the registry.
    void store(std::string key, void* lightUserData);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_REGISTRY_H_

