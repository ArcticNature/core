// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <exception>
#include <string>

#include "core/utility/lookup-table.h"


using sf::core::utility::LookupTable;


class DuplicateKey : public std::exception {
 public:
  std::string value;
  DuplicateKey(std::string val) {
    this->value = val;
  }
};


class NotFound : public std::exception {
 public:
  std::string value;
  NotFound(std::string val) {
    this->value = val;
  }
};


typedef LookupTable<int, DuplicateKey, NotFound> TestTable;


TEST(LookupTable, Add) {
  TestTable table;
  table.add("a", 42);
  ASSERT_EQ(42, table["a"]);
}

TEST(LookupTable, AddTwice) {
  TestTable table;
  table.add("a", 42);
  ASSERT_THROW(table.add("a", 21), DuplicateKey);
  ASSERT_EQ(42, table["a"]);
}

TEST(LookupTable, Get) {
  TestTable table;
  table.add("a", 42);
  ASSERT_EQ(42, table.get("a"));
}

TEST(LookupTable, GetMissing) {
  TestTable table;
  ASSERT_THROW(table.get("a"), NotFound);
}

TEST(LookupTable, Remove) {
  TestTable table;
  table.add("a", 42);
  table.remove("a");
  ASSERT_THROW(table.remove("a"), NotFound);
}

TEST(LookupTable, RemoveMissing) {
  TestTable table;
  ASSERT_THROW(table.remove("a"), NotFound);
}
