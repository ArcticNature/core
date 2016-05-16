// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/bin/client/input.h"
#include "core/context/client.h"
#include "core/model/event.h"

using sf::core::bin::ExecuteString;
using sf::core::context::Client;
using sf::core::model::EventRef;


class ExecuteStringTest : public ::testing::Test {
 public:
  std::stringstream output;

  ~ExecuteStringTest() {
    Client::destroy();
  }

  void execute(std::string code) {
    EventRef event(new ExecuteString(code, 1, &this->output));
    event->handle();
  }
};


TEST_F(ExecuteStringTest, DefinesGlobal) {
  this->execute("test = 5");
  ASSERT_EQ(5, Client::lua().globals()->toInt("test"));
  ASSERT_EQ(0, Client::lua().stack()->size());
}

TEST_F(ExecuteStringTest, SupportTopLevelExpression) {
  this->execute("2 + 4");
  ASSERT_EQ(0, Client::lua().stack()->size());
}

TEST_F(ExecuteStringTest, ResultIsPrinted) {
  this->execute("return 2 + 4");
  ASSERT_EQ("6\n", this->output.str());
  ASSERT_EQ(0, Client::lua().stack()->size());
}

TEST_F(ExecuteStringTest, ResultsArePrinted) {
  this->execute("function test() return 2, 4 end");
  ASSERT_EQ("", this->output.str());
  this->output.str("");

  this->execute("return test()");
  ASSERT_EQ("2\t4\n", this->output.str());
  ASSERT_EQ(0, Client::lua().stack()->size());
}
