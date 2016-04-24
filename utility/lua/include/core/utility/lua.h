// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_H_
#define CORE_UTILITY_LUA_H_

#include <memory>

#include "lua.hpp"

namespace sf {
namespace core {
namespace utility {

  // Forward declare classes needed by the lua wrapper.
  class LuaRegistry;
  class LuaStack;


  //! Wrapper for a LUA state.
  class Lua {
   protected:
    friend LuaRegistry;
    friend LuaStack;

    std::shared_ptr<lua_State> state;
    std::shared_ptr<LuaRegistry> _registry;
    std::shared_ptr<LuaStack> _stack;

    //! Checks if the previous operation failed.
    void checkError(int code, std::string name = "<block>");

   public:
    Lua();
    virtual ~Lua();

    //! Runs the given code.
    void doString(std::string code, std::string name = "<block>");

    //! Returns the state registry.
    LuaRegistry* registry();

    //! Returns a proxy to manipulate the stack.
    LuaStack* stack();

   public:
    //! Returns a pointer to the Lua wrapping the given state.
    static Lua* fetchFrom(lua_State* state);
  };


  //! Helper class to manipulate a Lua stack.
  class LuaStack {
   protected:
    Lua* state;

   public:
    LuaStack(Lua* state);

    //! Push a value onto the stack.
    void push(std::string value);

    //! Returns the current size of the stack.
    int size();

    //! Returns (and optionally pops) a string.
    std::string toString(int index = -1, bool pop = false);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_H_
