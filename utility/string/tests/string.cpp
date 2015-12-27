// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/utility/string.h"


using sf::core::utility::string::currentTimestamp;
using sf::core::utility::string::findFirsNotEscaped;
using sf::core::utility::string::join;
using sf::core::utility::string::split;
using sf::core::utility::string::toString;


bool checkDigits(std::string text, int start, int length) {
  for (int idx = start; idx < start + length; idx++) {
    int digit = text[idx] - '0';
    EXPECT_TRUE(digit >= 0 || digit < 10);
  }
}


TEST(StringUtility, TimeStamp) {
  std::string time = currentTimestamp();

  // Digits.
  checkDigits(time, 0, 4);
  checkDigits(time, 5, 2);
  checkDigits(time, 8, 2);
  checkDigits(time, 11, 2);
  checkDigits(time, 14, 2);
  checkDigits(time, 17, 2);

  // Separators.
  ASSERT_EQ('-', time[4]);
  ASSERT_EQ('-', time[7]);
  ASSERT_EQ(' ', time[10]);
  ASSERT_EQ(':', time[13]);
  ASSERT_EQ(':', time[16]);
}


TEST(StringUtility, Escape) {
  ASSERT_EQ(-1, findFirsNotEscaped("a", "\\a\\a\\a"));
  ASSERT_EQ(-1, findFirsNotEscaped("abc", "\\abc"));
}

TEST(StringUtility, Match) {
  ASSERT_EQ(0, findFirsNotEscaped("a", "a"));
  ASSERT_EQ(0, findFirsNotEscaped("a", "aaa"));
  ASSERT_EQ(2, findFirsNotEscaped("a", "cba"));
  ASSERT_EQ(2, findFirsNotEscaped("a", "cbaabc"));
  ASSERT_EQ(2, findFirsNotEscaped("abc", "\\\\abc"));
  ASSERT_EQ(4, findFirsNotEscaped("abc", "\\abcabc"));
}

TEST(StringUtility, NoMatch) {
  ASSERT_EQ(-1, findFirsNotEscaped("a", ""));
  ASSERT_EQ(-1, findFirsNotEscaped("a", "bcdef"));
  ASSERT_EQ(-1, findFirsNotEscaped("abc", "ab"));
}


TEST(StringUtility, Join) {
  std::vector<std::string> parts = {{"a"}, {"b"}, {"c"}};
  std::string result = join(parts, '-', 3);
  ASSERT_EQ("a-b-c", result);
}

TEST(StringUtility, Split) {
  std::string source = "a-b-c";
  std::vector<std::string> result = split(source, '-', 2);
  ASSERT_EQ(2, result.size());
  ASSERT_EQ("a", result[0]);
  ASSERT_EQ("b-c", result[1]);
}


TEST(StringUtility, ToString) {
  ASSERT_EQ("42", toString(42));
  ASSERT_EQ("-42", toString(-42));
}
