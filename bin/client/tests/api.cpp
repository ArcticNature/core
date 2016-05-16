// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/bin/client/api.h"
#include "core/bin/client/input.h"
#include "core/context/client.h"
#include "core/exceptions/base.h"

#include "core/lifecycle/client.h"
#include "core/model/event.h"
#include "core/utility/lua.h"
#include "core/utility/lua/table.h"

using sf::core::bin::ExecuteString;
using sf::core::context::Client;
using sf::core::exception::CleanExit;

using sf::core::lifecycle::ClientLifecycle;
using sf::core::model::EventRef;
using sf::core::utility::Lua;
using sf::core::utility::LuaTable;


class ClientLuaBindingTest : public ::testing::Test {
 public:
  std::stringstream output;

  ClientLuaBindingTest() {
    ClientLifecycle::Lua::Init();
  }

  ~ClientLuaBindingTest() {
    Client::destroy();
  }

  void execute(std::string code) {
    EventRef event(new ExecuteString(code, 1, &this->output));
    event->handle();
  }
};


TEST_F(ClientLuaBindingTest, ClientExists) {
  Lua lua = Client::lua();
  lua.globals()->toStack("client");
  ASSERT_EQ(LUA_TUSERDATA, lua.stack()->type());
}

TEST_F(ClientLuaBindingTest, Exits) {
  Lua lua = Client::lua();
  ASSERT_THROW(lua.doString("client:exit()"), CleanExit);
}

TEST_F(ClientLuaBindingTest, Version) {
  Lua lua = Client::lua();
  lua.doString("return client:version()");
  LuaTable version(&lua, -1, true);
  version.toString("number");
  version.toString("hash");
  version.toString("taint");
}
