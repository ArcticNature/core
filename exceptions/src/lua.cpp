// Copyright 2016 Stefano Pogliani <stafano@spogliani.net>
#include "core/exceptions/lua.h"

#include <string>


using sf::core::exception::LuaException;
using sf::core::exception::LuaGCError;
using sf::core::exception::LuaHandlerError;
using sf::core::exception::LuaInvalidState;
using sf::core::exception::LuaMemoryError;
using sf::core::exception::LuaRuntimeError; 
using sf::core::exception::LuaSyntaxError;
using sf::core::exception::LuaTypeError;
using sf::core::exception::LuaValueError;
using sf::core::exception::SfException;


LuaException::LuaException(std::string msg) : SfException(msg) {}


LuaRuntimeError::LuaRuntimeError(std::string message) : LuaException(
    "Lua runtime error: " + message
) { }

int LuaRuntimeError::getCode() const {
  return -1004;
}

LuaSyntaxError::LuaSyntaxError(std::string name) : LuaException(
    "Syntax error while loading '" + name + "'"
) { }

int LuaSyntaxError::getCode() const {
  return -1005;
}

LuaTypeError::LuaTypeError(
    std::string expected, std::string actual
) : LuaException(
  "Expected type " + expected + " while " + actual + " found."
) {
  // NOOP.
}

int LuaTypeError::getCode() const {
  return -1006;
}

LuaValueError::LuaValueError(
    std::string type, std::string value
) : LuaException(
  "Lua value ('" + value + "') has an unexpected type '" + type + "'."
) {
  // NOOP.
}

int LuaValueError::getCode() const {
  return -1007;
}


NO_ARG_DEFINITION(
    LuaException, LuaGCError, -1000,
    "Error while running the garbage collector."
);
NO_ARG_DEFINITION(
    LuaException, LuaHandlerError, -1001,
    "Error while processing a previous error."
);
NO_ARG_DEFINITION(
    LuaException, LuaInvalidState, -1002,
    "The given LUA state is not valid."
);
NO_ARG_DEFINITION(
    LuaException, LuaMemoryError, -1003,
    "Unable to allocate the required memory."
);
