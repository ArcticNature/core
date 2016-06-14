// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/exceptions/base.h"

#include "core/model/cli-parser.h"
#include "core/model/repository.h"
#include "core/registry/repositories.h"

using sf::core::context::Static;
using sf::core::exception::FactoryNotFound;

using sf::core::model::CLIParser;
using sf::core::model::Repository;
using sf::core::model::RepositoryRef;
using sf::core::model::RepositoryVersionRef;
using sf::core::registry::ReposRegistry;


class TestRepo : public Repository {
 public:
  RepositoryVersionRef latest() {
    return RepositoryVersionRef();
  }

  RepositoryVersionRef version(std::string) {
    return RepositoryVersionRef();
  }

  std::string resolveVersion(const std::string version) {
    return version;
  }

  void verifyVersion(const std::string version) {
    // Noop.
  }
};

RepositoryRef make_test_repo() {
  return RepositoryRef(new TestRepo());
}


class TestParser : public CLIParser {
 protected:
  void parseLogic(int* argc, char*** argv) {
    this->setString("repo-type", "test");
    this->setString("repo-path", "/path/to/repo");
  }
};


class ReposRegistryTest : public ::testing::Test {
 public:
  ReposRegistryTest() {
    CLIParser* parser = new TestParser();
    Static::parser(parser);
    CLIParser::miscOptions(parser);
    parser->parse(nullptr, nullptr);
    ReposRegistry::RegisterFactory("test", make_test_repo);
  }

  ~ReposRegistryTest() {
    ReposRegistry::destroy();
    Static::destroy();
  }
};


TEST_F(ReposRegistryTest, FailIfNoType) {
  Static::parser()->setString("repo-type", "null");
  ASSERT_THROW(ReposRegistry::initStaticContext(), FactoryNotFound);
}

TEST_F(ReposRegistryTest, Initialse) {
  ReposRegistry::initStaticContext();
  ASSERT_NE(nullptr, Static::repository().get());
}
