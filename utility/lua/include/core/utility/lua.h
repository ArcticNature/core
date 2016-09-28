// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_H_
#define CORE_UTILITY_LUA_H_

#include <istream>
#include <memory>
#include <ostream>
#include <string>

#include "lua.hpp"

#include "core/exceptions/lua.h"
#include "core/utility/lua/table.h"

namespace sf {
namespace core {
namespace utility {

  // Forward declare classes needed by the lua wrapper.
  class LuaArguments;
  class LuaRegistry;
  class LuaStack;
  class LuaTypeProxy;
  class LuaUpvalues;


  //! Wrapper for a LUA state.
  class Lua {
   protected:
    friend LuaArguments;
    friend LuaRegistry;
    friend LuaStack;
    friend LuaTable;
    friend LuaTypeProxy;
    friend LuaUpvalues;

    std::shared_ptr<lua_State> state;
    std::shared_ptr<LuaRegistry> _registry;
    std::shared_ptr<LuaStack> _stack;
    std::shared_ptr<LuaTable> _globals;

    //! Store the last given chunk name for error messages.
    std::string last_name;

    //! Checks if the previous operation failed.
    void checkError(int code, std::string name = "<block>");

   public:
    Lua();
    virtual ~Lua();

    //! Calls the LUA function on top of the stack.
    /*!
     * See lua_pcall documentation.
     * \param nargs Number of arguments on the stack for the call.
     * \param nresults Number of results to expect.
     * \param msgh Index on the stack of the optional message handler.
     * \param clear Indicates that the stack schould be cleared
     *              if and exception is thrown.
     */
    void call(
        int nargs, int nresults = LUA_MULTRET,
        int msgh = 0, bool clear = true
    );

    //! Runs code from the given input stream.
    void doStream(std::shared_ptr<std::istream> stream, std::string name);

    //! Runs the given code code.
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


  //! Helper class to manipulate Lua call arguments.
  class LuaArguments {
   protected:
    Lua* state;

   public:
    explicit LuaArguments(Lua* state);

    //! Checks that an argument of any type is passed.
    void any(int number);

    //! Checks that the argument is a boolean and returns it.
    bool boolean(int number);

    //! Returns a reference to the given argument.
    lua_Integer reference(int number);

    //! Checks that the argument is a table and returns it.
    LuaTable table(int number);
  };


  //! Helper class to manipulate a Lua stack.
  class LuaStack {
   protected:
    Lua* state;
    std::shared_ptr<LuaArguments> _arguments;

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

    //! Returns an object able to manipulate arguments.
    LuaArguments* arguments();

    //! Checks the stack can hold count more items.
    void check(int count);

    //! Removes all values from the stack.
    void clear();

    //! Duplicates an item by pushing it to the top.
    void duplicate(int index);

    //! Pushes a new table on top of the stack and returns a wrapper to it.
    LuaTable newTable(bool pop = false);

    //! Pritty prints a value on the stack to a stream.
    void pprint(std::ostream* out_stream, int index = -1, int nesting = 0);

    //! Prints the values on the stack to a stream.
    void print(std::ostream* out_stream);

    //! Pushes an integer onto the stack.
    void push(int value);

    //! Pushes a string onto the stack.
    void push(std::string value);

    //! Pushes a closure onto the stack.
    void push(lua_CFunction value, int close_with);

    //! Pushes a boolean onto the stack.
    void pushBool(bool value);

    //! Pushes nil onto the stack.
    void pushNil();

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

    //! Returns (and optionally pops) a boolean.
    bool toBoolean(int index = -1, bool pop = false);

    //! Returns (and optionally pops) an int.
    int toInt(int index = -1, bool pop = false);

    //! Returns (and optionally pops) a string.
    std::string toString(int index = -1, bool pop = false);

    //! Returns the type of the value on the stack.
    int type(int index = -1);

    //! Returns an interface to lua upvalues.
    LuaUpvalues upvalues();
  };


  //! Helper class to access Lua upvalues.
  class LuaUpvalues {
   protected:
    Lua* state;

   public:
    explicit LuaUpvalues(Lua* state);

    //! Returns a light userdata pointer stored in an upvalue.
    template <typename type>
    type* toLightUserdata(int idx) {
      int full_idx = lua_upvalueindex(idx);
      lua_State* state = this->state->state.get();
      if (this->state->stack()->type(full_idx) != LUA_TLIGHTUSERDATA) {
        throw sf::core::exception::LuaTypeError(
            "lightuserdata",
            lua_typename(state, lua_type(state, idx))
        );
      }
      void* pointer = lua_touserdata(state, full_idx);
      return reinterpret_cast<type*>(pointer);
    };
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_H_
