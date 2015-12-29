// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/registry/cli-parser.h"

#include <string>

#define DEFAULT_PARSER "gflags"

using sf::core::registry::CLIParsers;


std::string CLIParsers::nameParser(int argc, char** argv) {
  if (argc <= 1) {
    return DEFAULT_PARSER;
  }

  std::string may_be_parser(argv[1]);
  if (may_be_parser.substr(0, 9) != "--parser=") {
    return DEFAULT_PARSER;
  }

  std::string parser = may_be_parser.substr(9);
  if (parser == "") {
    return DEFAULT_PARSER;
  }

  return parser;
}
