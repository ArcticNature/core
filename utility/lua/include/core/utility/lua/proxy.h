// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_PROXY_H_
#define CORE_UTILITY_LUA_PROXY_H_

#include <string>
#include "lua.hpp"

#include "core/utility/lua.h"


namespace sf {
namespace core {
namespace utility {

  //! Type of instance cleanup functions.
  /*!
   * LuaTypeProxy registers a __gc metamethod that
   * will invoke a lua_proxy_delete function passing the instance
   * as the first argument.
   */
  typedef void (*lua_proxy_delete)(void* instance);

  //! Helper proxy a C++ instance through a LUA table.
  /*
   * LuaTypeProxy subclases define LUA to C++ proxy methods and
   * register them into a new MetaTable into a Lua environment.
   */
  class LuaTypeProxy {
   protected:
    lua_State* _state;

    //! Type use constructor.
    /*!
     * This constructor is used when a LuaTypeProxy instance
     * is created to manipulate a LUA userdata bound to this
     * proxy type.
     *
     * It is needed when methods like `checkArgument` are used.
     */
    explicit LuaTypeProxy(Lua lua);
    explicit LuaTypeProxy(lua_State* state);

    //! Checks the first argument on the lua stack and returns it.
    void* check() const;

    template<typename type>
    type* check() const {
      void* value = this->check();
      return reinterpret_cast<type*>(value);
    }

    //! Returns the LUA state if this is a `type use` instance.
    lua_State* state() const;

    //! Returns the unique identifier for this proxy type.
    virtual std::string typeId() const = 0;

    //! Returns the delete function to use for this type.
    virtual lua_proxy_delete deleter() const = 0;

    //! Returns an array of LUA C functions for the type.
    /*!
     * This is the second argument to a `luaL_setfuncs` call.
     * See https://www.lua.org/manual/5.3/manual.html#luaL_setfuncs
     */
    virtual const luaL_Reg* library() const = 0;

   public:
    //! Type definition constructor.
    /*!
     * This constructor is used when a LuaTypeProxy instance
     * is created to define an new type (`initType` method) or
     * when a type instance is bound to the proxy type
     * (`bind` method).
     */
    LuaTypeProxy();
    virtual ~LuaTypeProxy();

    //! Registers the type metatable in Lua.
    void initType(Lua lua);

    //! Creates a LUA userdata with this type.
    /*!
     * The instance is now owned by the LUA userdata and is
     * bound to it.
     *
     * Specifically, memory de-allocation is performed by the
     * LuaTypeProxy deleter invoked by the LUA garbage collector.
     *
     * The new userdata is left on the stack.
     */
    void bind(Lua lua, void* instance);

    //! Creates a LUA userdata with this type.
    template<typename type>
    void bind(Lua lua, type* instance) {
      this->bind(lua, reinterpret_cast<void*>(instance));
    }

   protected:
    //! LUA garbage collector callback for proxyed instances.
    static int lua_proxy_gc(lua_State* state);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_PROXY_H_
