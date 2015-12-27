// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/model/options.h"
#include "core/exceptions/options.h"

using sf::core::exception::VariableNotFound;
using sf::core::model::Options;


TEST(Options, Boolean) {
  Options options;
  options.setBoolean("test.true",  true);
  options.setBoolean("test.false", false);

  ASSERT_TRUE(options.getBoolean("test.true"));
  ASSERT_FALSE(options.getBoolean("test.false"));
}

TEST(Options, Integer) {
  Options options;
  options.setInteger("zero", 0);
  options.setInteger("one", 1);
  options.setInteger("negative", -22);

  ASSERT_EQ(0, options.getInteger("zero"));
  ASSERT_EQ(1, options.getInteger("one"));
  ASSERT_EQ(-22, options.getInteger("negative"));
}

TEST(Options, String) {
  Options options;
  options.setString("empty", "");
  options.setString("short", "short");

  ASSERT_EQ("", options.getString("empty"));
  ASSERT_EQ("short", options.getString("short"));
}

TEST(Options, ThorwsVariableNotFound) {
  Options options;
  ASSERT_THROW(options.getBoolean("not.a.var"), VariableNotFound);
  ASSERT_THROW(options.getInteger("not.a.var"), VariableNotFound);
  ASSERT_THROW(options.getString("not.a.var"), VariableNotFound);
}
