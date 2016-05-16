// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_H_
#define CORE_UTILITY_LUA_H_

#include <string>
#include <memory>

#include "lua.hpp"

#include "core/utility/lua/table.h"

namespace sf {
namespace core {
namespace utility {

  // Forward declare classes needed by the lua wrapper.
  class LuaRegistry;
  class LuaStack;
  class LuaTypeProxy;


  //! Wrapper for a LUA state.
  class Lua {
   protected:
    friend LuaRegistry;
    friend LuaStack;
    friend LuaTable;
    friend LuaTypeProxy;

    std::shared_ptr<lua_State> state;
    std::shared_ptr<LuaRegistry> _registry;
    std::shared_ptr<LuaStack> _stack;
    std::shared_ptr<LuaTable> _globals;

    //! Checks if the previous operation failed.
    void checkError(int code, std::string name = "<block>");

   public:
    Lua();
    virtual ~Lua();

    //! Runs the given code.
    void doString(std::string code, std::string name = "<block>");

    //! Returns a wrapper to the globals table.
    LuaTable* globals();

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
    explicit LuaStack(Lua* state);

    //! Returns the absolute index for the given index.
    /*!
     * Positive (absolute) indexes and pseudo indexes
     * are returned unchanged.
     *
     * Invalid indexes cause an exception to be thrown.
     */
    int absoluteIndex(int index);

    //! Checks the stack can hold count more items.
    void check(int count);

    //! Removes all values from the stack.
    void clear();

    //! Duplicates an item by pushing it to the top.
    void duplicate(int index);

    //! Pushes a new table on top of the stack and returns a wrapper to it.
    LuaTable newTable(bool pop = false);

    //! Pushes an integer onto the stack.
    void push(int value);

    //! Pushes a string onto the stack.
    void push(std::string value);

    //! Pushes a closure onto the stack.
    void push(lua_CFunction value, int close_with);

    //! Pushes a light userdata onto the stack.
    /*!
     * This method does not overload the `push` method because
     * it would make that method always valid, even when it is
     * not doing what you would expect.
     *
     * If you want to push a void* on the stack, you
     * need to confirm it by using a different name.
     */
    template<typename type>
    void pushLight(type* value) {
      lua_State* state = this->state->state.get();
      lua_checkstack(state, 1);
      lua_pushlightuserdata(state, reinterpret_cast<void*>(value));
    }

    //! Removes an item from the stack.
    void remove(int index);

    //! Returns the string repesentation of a value.
    std::string represent(int index);

    //! Returns the current size of the stack.
    int size();

    //! Returns (and optionally pops) an int.
    int toInt(int index = -1, bool pop = false);

    //! Returns (and optionally pops) a string.
    std::string toString(int index = -1, bool pop = false);

    //! Returns the type of the value on the stack.
    int type(int index = -1);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_H_
