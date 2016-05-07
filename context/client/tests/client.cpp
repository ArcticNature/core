// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/client.h"

using sf::core::context::Client;
using sf::core::utility::Lua;


class ClientTest : public ::testing::Test {
 public:
  ~ClientTest() {
    Client::destroy();
  }
};


TEST_F(ClientTest, SIngletonLua) {
  Lua lua1 = Client::lua();
  Lua lua2 = Client::lua();
  lua1.globals()->set("test", 4);
  ASSERT_EQ(4, lua2.globals()->toInt("test"));
}
