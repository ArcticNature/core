// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_TABLE_H_
#define CORE_UTILITY_LUA_TABLE_H_

#include <string>
#include "lua.hpp"

namespace sf {
namespace core {
namespace utility {

  // Forward declare lua state wrapper.
  class Lua;


  //! Wrapper class for lua tables.
  class LuaTable {
   protected:
    Lua* state;
    lua_Integer table_ref;

   public:
    //! Wrap the lua table at the given index.
    explicit LuaTable(Lua* state, int index = -1, bool pop = false);
    virtual ~LuaTable();

    //! Sets a table key to a value on the stack.
    void fromStack(std::string key, int index = -1);
    void fromStack(int key, int index = -1);

    //! Pushes a table key on the stack.
    void toStack(std::string key);
    void toStack(int key);

    //! Stores a value in the table.
    void set(int key, int value);
    void set(int key, std::string value);
    void set(std::string key, int value);
    void set(std::string key, std::string value);
    void set(std::string key, lua_CFunction value);

    //! Gets an int from a table.
    int toInt(int key);
    int toInt(std::string key);

    //! Gets a string from a table.
    std::string toString(int key);
    std::string toString(std::string key);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_TABLE_H_
