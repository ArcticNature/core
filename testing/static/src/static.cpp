// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/testing/static.h"

#include "core/context/static.h"
#include "core/interface/posix.h"
#include "core/model/cli-parser.h"


using sf::core::context::Static;
using sf::core::interface::Posix;
using sf::core::model::CLIParser;

using sf::core::testing::StaticContextTest;


//! Noop parser that does not process command line args.
class NoopCLIParser : public CLIParser {
 protected:
  void parseLogic(int* argc, char*** argv) {
    // Noop.
  }
};


StaticContextTest::StaticContextTest() {
  Static::initialise(new Posix());
  Static::parser(new NoopCLIParser());
}

StaticContextTest::~StaticContextTest() {
  Static::destroy();
}
