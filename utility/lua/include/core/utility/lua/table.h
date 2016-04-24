// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_TABLE_H_
#define CORE_UTILITY_LUA_TABLE_H_

namespace sf {
namespace core {
namespace utility {

  // Forward declare lua state wrapper.
  class Lua;

  //! Wrapper class for lua tables.
  class LuaTable {
   public:
    //! Wrap the lua table at the given index.
    explicit LuaTable(Lua state, int index = -1);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_TABLE_H_
