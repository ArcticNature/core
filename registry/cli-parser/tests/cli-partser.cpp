// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/registry/cli-parser.h"

using sf::core::registry::CLIParsers;


TEST(CLIParsers, ReturnsDefaultWithNoArgs) {
  std::string name = CLIParsers::nameParser(0, nullptr);
  ASSERT_EQ("gflags", name);
}

TEST(CLIParsers, ReturnsDefaultWithArgs) {
  char** args = new char*[2];
  args[0] = strdup("some-bin");
  args[1] = strdup("some-arg");

  std::string name = CLIParsers::nameParser(2, args);
  EXPECT_EQ("gflags", name);

  delete args[0];
  delete args[1];
  delete [] args;
}

TEST(CLIParsers, ReturnsDefaultWithWrongParser) {
  char** args = new char*[3];
  args[0] = strdup("some-bin");
  args[1] = strdup("--parser");
  args[2] = strdup("no");

  std::string name = CLIParsers::nameParser(3, args);
  EXPECT_EQ("gflags", name);

  delete args[0];
  delete args[1];
  delete args[2];
  delete [] args;
}

TEST(CLIParsers, ReturnsDefaultWithEmptyParser) {
  char** args = new char*[2];
  args[0] = strdup("some-bin");
  args[1] = strdup("--parser=");

  std::string name = CLIParsers::nameParser(2, args);
  EXPECT_EQ("gflags", name);

  delete args[0];
  delete args[1];
  delete [] args;
}

TEST(CLIParsers, ReturnsParser) {
  char** args = new char*[2];
  args[0] = strdup("some-bin");
  args[1] = strdup("--parser=abc");

  std::string name = CLIParsers::nameParser(2, args);
  EXPECT_EQ("abc", name);

  delete args[0];
  delete args[1];
  delete [] args;
}
