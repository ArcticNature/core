// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/utility/scored-list.h"

using sf::core::utility::ScoredList;


typedef ScoredList<int> TestList;

bool filter_even_values(int value) {
  return (value % 2) == 0;
}


TEST(ScoredList, Empty) {
  TestList list;
  ASSERT_EQ(0, list.pop().size());
  ASSERT_EQ(0, list.pop().size());
}

TEST(ScoredList, FreeRemainingBuckets) {
  TestList list;
  std::vector<int> values;
  list.insert(1, 4);
  list.insert(2, 5);
  list.insert(3, 6);
}

TEST(ScoredList, Insert) {
  TestList list;
  list.insert(0, 4);
  std::vector<int> values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(4, values[0]);
}

TEST(ScoredList, InsertAfter) {
  TestList list;
  std::vector<int> values;
  list.insert(1, 4);
  list.insert(2, 5);

  values = list.pop();
  ASSERT_EQ(0, values.size());

  values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(4, values[0]);

  values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(5, values[0]);
}

TEST(ScoredList, InsertBefore) {
  TestList list;
  std::vector<int> values;
  list.insert(2, 5);
  list.insert(1, 4);

  values = list.pop();
  ASSERT_EQ(0, values.size());

  values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(4, values[0]);

  values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(5, values[0]);
}

TEST(ScoredList, OffByOneErrors) {
  TestList list;
  std::vector<int> values;
  list.insert(2, 5);
  list.insert(1, 4);
  list.insert(3, 6);

  values = list.pop();
  ASSERT_EQ(0, values.size());

  values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(4, values[0]);

  values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(5, values[0]);

  values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(6, values[0]);
}

TEST(ScoredList, PopReducesScore) {
  TestList list;
  list.insert(1, 4);
  list.pop();
  std::vector<int> values = list.pop();
  ASSERT_EQ(1, values.size());
  ASSERT_EQ(4, values[0]);
}

TEST(ScoredList, FilterEvenValues) {
  TestList list;
  list.insert(1, 4);
  list.insert(2, 5);
  list.insert(3, 6);
  list.insert(3, 8);

  std::vector<TestList::ScoredValue> evens = list.filter(filter_even_values);
  ASSERT_EQ(3, evens.size());
  ASSERT_EQ(1, evens[0].score);
  ASSERT_EQ(4, evens[0].value);
  ASSERT_EQ(3, evens[1].score);
  ASSERT_EQ(6, evens[1].value);
  ASSERT_EQ(3, evens[2].score);
  ASSERT_EQ(8, evens[2].value);
}
