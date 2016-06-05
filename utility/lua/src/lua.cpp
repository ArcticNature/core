// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/lua.h"

#include <string>

#include "core/exceptions/lua.h"
#include "core/utility/lua/registry.h"

#define LUA_SELF_REF_KEY "_sf_lua_self_ref"

using sf::core::exception::LuaGCError;
using sf::core::exception::LuaHandlerError;
using sf::core::exception::LuaInvalidState;
using sf::core::exception::LuaMemoryError;
using sf::core::exception::LuaRuntimeError;
using sf::core::exception::LuaSyntaxError;
using sf::core::exception::LuaTypeError;
using sf::core::exception::SfException;

using sf::core::utility::Lua;
using sf::core::utility::LuaArguments;
using sf::core::utility::LuaRegistry;
using sf::core::utility::LuaStack;
using sf::core::utility::LuaTable;


Lua* Lua::fetchFrom(lua_State* state) {
  // Attempt to fetch Lua pointer.
  lua_pushstring(state, LUA_SELF_REF_KEY);
  lua_rawget(state, LUA_REGISTRYINDEX);
  if (!lua_islightuserdata(state, -1)) {
    throw LuaInvalidState();
  }

  // Fetch and cast the value on the stack.
  void* address = lua_touserdata(state, -1);
  lua_pop(state, 1);
  return static_cast<Lua*>(address);
}


void Lua::checkError(int code, std::string source_name) {
  switch (code) {
    case LUA_OK:        return;
    case LUA_ERRERR:    throw LuaHandlerError();
    case LUA_ERRGCMM:   throw LuaGCError();
    case LUA_ERRMEM:    throw LuaMemoryError();
    case LUA_ERRSYNTAX: throw LuaSyntaxError(source_name);
    case LUA_ERRRUN:
      throw LuaRuntimeError(this->stack()->toString(-1, true));
    default:
      throw LuaRuntimeError("Invalid state code.");
  }
}


Lua::Lua() : state(luaL_newstate(), lua_close) {
  // Initialise state and registry.
  this->_registry = std::shared_ptr<LuaRegistry>(new LuaRegistry(this));
  this->_stack = std::shared_ptr<LuaStack>(new LuaStack(this));

  // Create LuaTable for globals.
  this->_registry->dereference(LUA_RIDX_GLOBALS);
  this->_globals = std::shared_ptr<LuaTable>(new LuaTable(this, -1));
  this->_stack->remove(-1);

  // Store a reference to myself in the registry.
  this->_registry->store(LUA_SELF_REF_KEY, this);
}

Lua::~Lua() {
  this->_globals  = std::shared_ptr<LuaTable>();
  this->_stack    = std::shared_ptr<LuaStack>();
  this->_registry = std::shared_ptr<LuaRegistry>();
}

void Lua::call(int nargs, int nresults, int msgh, bool clear) {
  lua_State* state = this->state.get();
  // Clear the stack in case of errors.
  try {
    int result = lua_pcall(state, nargs, nresults, msgh);
    this->checkError(result, this->last_name);
  } catch(SfException& ex) {
    if (clear) {
      this->stack()->clear();
    }
    throw;
  }
}

void Lua::doString(std::string code, std::string name) {
  int result = 0;
  lua_State* state = this->state.get();
  this->last_name  = name;

  result = luaL_loadbuffer(state, code.c_str(), code.length(), name.c_str());
  this->checkError(result, name);
  this->call(0, LUA_MULTRET, 0, true);
}

LuaTable* Lua::globals() {
  return this->_globals.get();
}

LuaRegistry* Lua::registry() {
  return this->_registry.get();
}

LuaStack* Lua::stack() {
  return this->_stack.get();
}


LuaArguments::LuaArguments(Lua* state) {
  this->state = state;
}

void LuaArguments::any(int number) {
  luaL_checkany(this->state->state.get(), number);
}

lua_Integer LuaArguments::reference(int number) {
  this->state->stack()->duplicate(number);
  return this->state->registry()->referenceTop();
}


LuaStack::LuaStack(Lua* state) {
  this->state = state;
  this->_arguments = std::shared_ptr<LuaArguments>(new LuaArguments(state));
}

int LuaStack::absoluteIndex(int index) {
  int size = this->size();
  bool valid_abs = index > 0 && index <= size;
  bool valid_rel = index < 0 && index >= -size;
  bool valid_pseudo = index <= LUA_REGISTRYINDEX;
  bool valid = valid_abs || valid_rel || valid_pseudo;
  if (!valid) {
    throw LuaInvalidState();
  }

  if (valid_abs || valid_pseudo) {
    return index;
  }
  return size + index + 1;
}

LuaArguments* LuaStack::arguments() {
  return this->_arguments.get();
}

void LuaStack::check(int count) {
  lua_checkstack(this->state->state.get(), count);
}

void LuaStack::clear() {
  lua_settop(this->state->state.get(), 0);
}

void LuaStack::duplicate(int index) {
  lua_pushvalue(this->state->state.get(), index);
}

LuaTable LuaStack::newTable(bool pop) {
  lua_State* state = this->state->state.get();
  lua_checkstack(state, 1);
  lua_newtable(state);
  return LuaTable(this->state, -1, pop);
}

void LuaStack::print(std::ostream* out_stream) {
  int top = this->size();
  if (top == 0) {
    return;  // Nothing to print.
  }

  std::ostream& out = *out_stream;
  for (int idx=1; idx <= top; idx++) {
    out << this->represent(idx);
    if (idx != top) {
      out << '\t';
    }
  }

  out << std::endl;
  this->clear();
}

void LuaStack::push(int value) {
  lua_State* state = this->state->state.get();
  lua_checkstack(state, 1);
  lua_pushinteger(state, value);
}

void LuaStack::push(std::string value) {
  lua_State* state = this->state->state.get();
  lua_checkstack(state, 1);
  lua_pushstring(state, value.c_str());
}

void LuaStack::push(lua_CFunction value, int close_with) {
  lua_State* state = this->state->state.get();
  lua_checkstack(state, 1);
  lua_pushcclosure(state, value, close_with);
}

void LuaStack::remove(int index) {
  lua_remove(this->state->state.get(), index);
}

std::string LuaStack::represent(int index) {
  lua_State*  state = this->state->state.get();
  const char* value = luaL_tolstring(state, index, nullptr);
  lua_remove(state, -1);
  return std::string(value);
}

int LuaStack::size() {
  return lua_gettop(this->state->state.get());
}

int LuaStack::toInt(int index, bool pop) {
  lua_State* state = this->state->state.get();
  if (lua_type(state, index) != LUA_TNUMBER || !lua_isinteger(state, index)) {
    throw LuaTypeError(
      "integer",
      lua_typename(state, lua_type(state, index))
    );
  }

  int value = lua_tointeger(state, index);
  if (pop) {
    lua_remove(state, index);
  }
  return value;
}

std::string LuaStack::toString(int index, bool pop) {
  lua_State* state = this->state->state.get();
  if (lua_type(state, index) != LUA_TSTRING) {
    throw LuaTypeError(
        "string",
        lua_typename(state, lua_type(state, index))
    );
  }

  std::string value = lua_tostring(state, index);
  if (pop) {
    lua_remove(state, index);
  }
  return value;
}

int LuaStack::type(int index) {
  return lua_type(this->state->state.get(), index);
}
