// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/registry/base.h"

using sf::core::exception::DuplicateInjection;
using sf::core::exception::FactoryNotFound;
using sf::core::registry::Registry;


typedef int*(*test_factory)();
typedef Registry<test_factory> TestRegistry;

int* make_one() {
  return new int(1);
}


TEST(Regisrty, CannotGetMissingFactory) {
  TestRegistry reg;
  ASSERT_THROW(reg.get("none"), FactoryNotFound);
}

TEST(Regisrty, CannotSetFactoryTwice) {
  TestRegistry reg;
  reg.registerFactory("one", make_one);
  ASSERT_THROW(reg.registerFactory("one", make_one), DuplicateInjection);
}

TEST(Regisrty, GetFactory) {
  TestRegistry reg;
  reg.registerFactory("one", make_one);

  test_factory factory = reg.get("one");
  int* result = factory();

  EXPECT_EQ(1, *result);
  delete result;
}

TEST(Registry, Singleton) {
  TestRegistry* reg1 = TestRegistry::instance();
  TestRegistry* reg2 = TestRegistry::instance();
  ASSERT_EQ(reg1, reg2);
}

TEST(Registry, SingletonProxy) {
  TestRegistry::RegisterFactory("one", make_one);
  test_factory factory = TestRegistry::Get("one");

  int* result = factory();
  EXPECT_EQ(1, *result);
  delete result;
}
