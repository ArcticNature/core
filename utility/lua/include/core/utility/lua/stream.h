// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LUA_STREAM_H_
#define CORE_UTILITY_LUA_STREAM_H_

#include <istream>
#include <memory>

#include "lua.hpp"

namespace sf {
namespace core {
namespace utility {

  //! Wrapper around std::shared_ptr<std::istreams> for LUA read.
  class LuaStreamReader {
   protected:
    char* buffer;
    int   buffer_size;
    std::shared_ptr<std::istream> source;

   public:
    LuaStreamReader(
        std::shared_ptr<std::istream> source, int buffer_size = 512
    );
    virtual ~LuaStreamReader();
    const char* readNext(size_t* size);

   public:
    static const char* read(lua_State*, void* data, size_t* size);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LUA_STREAM_H_
