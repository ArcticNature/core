// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_TABLE_H_
#define CORE_UTILITY_LUA_TABLE_H_

#include <json.hpp>
#include <lua.hpp>

#include <functional>
#include <string>

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

    // TODO(stefano): deprecate and replace with iterators.
    //! Iterate over elements in the table placing key and value on the stack.
    void forEach(std::function<void(int key)> fn);
    void forEach(std::function<void(std::string key)> fn);

    //! Sets a table key to a value on the stack.
    void fromStack(int key, int index = -1);
    void fromStack(std::string key, int index = -1);

    //! Checks the table for presence of a key.
    bool has(int key);
    bool has(std::string key);

    //! Returns a new table stored at the given key.
    LuaTable newTable(std::string key);
    LuaTable newTable(int key);

    //! Pushes a table key on the stack.
    void toStack(std::string key);
    void toStack(int key);

    //! Pushes itself on the stack.
    void pushMe();

    //! Removes a value by key.
    void remove(int key);
    void remove(std::string key);

    //! Stores a value in the table.
    void set(int key, int value);
    void set(int key, std::string value);
    void set(std::string key, int value);
    void set(std::string key, std::string value);
    void set(std::string key, lua_CFunction value);

    //! Sets the metatable for itself from the stack.
    void setMetatable(int index = -1);

    //! Converts the table into a JSON object.
    nlohmann::json toJSON();

    //! Gets an int from a table.
    int toInt(int key);
    int toInt(std::string key);

    //! Gets a string from a table.
    std::string toString(int key);
    std::string toString(std::string key);

    //! Gets a table from a table.
    LuaTable toTable(std::string key);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_TABLE_H_
