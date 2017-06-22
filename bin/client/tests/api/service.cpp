// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/bin/client/api.h"
#include "core/context/client.h"

#include "core/lifecycle/client.h"
#include "core/utility/lua.h"


using sf::core::context::Client;
using sf::core::lifecycle::ClientLifecycle;
using sf::core::utility::Lua;


class ServiceLuaTypeTest : public ::testing::Test {
 protected:
  ServiceLuaTypeTest() {
    ClientLifecycle::Lua::Init();
  }

  ~ServiceLuaTypeTest() {
    Client::destroy();
  }
};


TEST_F(ServiceLuaTypeTest, ServiceIsSet) {
  Lua lua = Client::lua();
  lua.globals()->toStack("service");
  ASSERT_EQ(LUA_TUSERDATA, lua.stack()->type());
}

TEST_F(ServiceLuaTypeTest, ServiceStartIsSet) {
  Lua lua = Client::lua();
  lua.doString("return service.start");
  ASSERT_EQ(LUA_TFUNCTION, lua.stack()->type());
}
