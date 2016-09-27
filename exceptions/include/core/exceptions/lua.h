// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_EXCEPTIONS_LUA_H_
#define CORE_EXCEPTIONS_LUA_H_

#include <string>

#include "core/exceptions/base.h"


#define START_LUA_EXCEPTION_GUARD try {
#define END_LUA_EXCEPTION_GUARD(lua_state)            \
  } catch (sf::exception::SfException& ex) {          \
    sf::core::model::LogInfo info = {                 \
      {"error", ex.what()},                           \
      {"trace", ex.getTrace()}                        \
    };                                                \
    ERRORV(                                           \
      sf::core::context::Context::logger(),           \
      "Caught exception in LUA CFunction: ${error}",  \
      info                                            \
    );                                                \
    lua_pushstring(lua_state, ex.what());             \
    lua_error(lua_state);                             \
  }


namespace sf {
namespace core {
namespace exception {

  class LuaException : public SfException {
   public:
    explicit LuaException(std::string msg);
  };

  class LuaTypeError : public LuaException {
   public:
    LuaTypeError(std::string expected, std::string actual);
    virtual int getCode() const;
  };

  class LuaValueError : public LuaException {
   public:
    LuaValueError(std::string type, std::string value);
    virtual int getCode() const;
  };

  NO_ARG_EXCEPTION(LuaException, LuaGCError);
  NO_ARG_EXCEPTION(LuaException, LuaHandlerError);

  //! Thrown when a LuaTypeProxy is used incorrectly.
  MSG_EXCEPTION(LuaException, LuaInvalidProxyUse);

  //! Thrown when an invalid LUA state is used.
  NO_ARG_EXCEPTION(LuaException, LuaInvalidState);

  //! Thrown when LUA cannot allocate memory.
  NO_ARG_EXCEPTION(LuaException, LuaMemoryError);

  //! Thrown when LUA raises a runtime error.
  MSG_EXCEPTION(LuaException, LuaRuntimeError);

  //! Thrown when LUA raises a syntax error.
  MSG_EXCEPTION(LuaException, LuaSyntaxError);

  //! Thrown when a LuaTypeProxy is defined twice.
  MSG_EXCEPTION(LuaException, LuaTypeExists);

  //! Thrown when a LuaTypeProxy is defined twice.
  MSG_EXCEPTION(LuaException, LuaTypeNotFound);

}  // namespace exception
}  // namespace core
}  // namespace sf

#endif  // CORE_EXCEPTIONS_LUA_H_

