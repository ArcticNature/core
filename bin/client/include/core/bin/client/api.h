// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_CLIENT_API_H_
#define CORE_BIN_CLIENT_API_H_

#include <string>

#include "core/utility/lua.h"
#include "core/utility/lua/proxy.h"

namespace sf {
namespace core {
namespace bin {

  //! Client API for LUA environment.
  class ClientLuaBinding {
   public:
    void exit();
  };


  //! Lua type of the client object.
  class ClientLuaType : public sf::core::utility::LuaTypeProxy {
   protected:
    static const struct luaL_Reg lib[];

    static void deleteInstance(void* instance);
    static int  exit(lua_State* state);
    static int  version(lua_State* state);

   protected:
    explicit ClientLuaType(lua_State* state);

    sf::core::utility::lua_proxy_delete deleter() const;
    const luaL_Reg* library() const;
    std::string typeId() const;

   public:
    ClientLuaType();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_CLIENT_API_H_
