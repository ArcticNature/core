// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/interface/config/node.h"
#include "core/model/cli-parser.h"
#include "core/model/repository.h"

#include "core/utility/lua.h"
#include "core/utility/lua/table.h"

#include "ext/repository/git.h"
#include "./base.h"


using sf::core::context::ContextRef;
using sf::core::context::Static;

using sf::core::interface::NodeConfigIntent;
using sf::core::interface::NodeConfigIntentRef;
using sf::core::interface::NodeConfigIntentLuaProxy;
using sf::core::interface::NodeConfigLoader;

using sf::core::model::CLIParser;
using sf::core::model::RepositoryRef;

using sf::core::utility::Lua;
using sf::core::utility::LuaTable;

using sf::core::test::NodeConfigLoaderTest;
using sf::core::test::NodeConfigIntentsOrderTest;
using sf::core::test::TestLoader;

using sf::ext::repository::GitRepo;


class TestParser : public CLIParser {
 protected:
  void parseLogic(int* argc, char*** argv) {
    this->setString("repo-type", "test");
    this->setString("repo-path", "../config-example");
  }
};


class MockIntent : public NodeConfigIntent {
 protected:
  std::string _provides;

 public:
  MockIntent(std::string id, std::string provides) : NodeConfigIntent(id) {
    this->_provides = provides;
  }

  std::vector<std::string> depends() const {
    return std::vector<std::string>();
  }

  std::string provides() const {
    return this->_provides;
  }

  void apply(ContextRef context) {
    // Noop.
  }

  void verify(ContextRef context) {
    // Noop.
  }
};

class Cycle1Intent : public MockIntent {
 public:
  Cycle1Intent() : MockIntent("cycle1", "cycle1") {
    // Noop.
  }

  std::vector<std::string> depends() const {
    std::vector<std::string> deps;
    deps.push_back("cycle2");
    return deps;
  }
};

class Cycle2Intent : public MockIntent {
 public:
  Cycle2Intent() : MockIntent("cycle2", "cycle2") {
    // Noop.
  }

  std::vector<std::string> depends() const {
    std::vector<std::string> deps;
    deps.push_back("cycle1");
    return deps;
  }
};

class EventManagerIntent : public MockIntent {
 public:
  EventManagerIntent() : MockIntent("event.manager", "event.manager") {
    // Noop.
  }
};

class EventTcpIntent : public MockIntent {
 public:
  EventTcpIntent() : MockIntent("event.tcp", "event.tcp") {
    // Noop.
  }

  std::vector<std::string> depends() const {
    std::vector<std::string> deps;
    deps.push_back("event.manager");
    return deps;
  }
};

class EventUnixIntent : public MockIntent {
 public:
  EventUnixIntent() : MockIntent("event.unix", "event.unix") {
    // Noop.
  }

  std::vector<std::string> depends() const {
    std::vector<std::string> deps;
    deps.push_back("event.manager");
    return deps;
  }
};

class NullIntent : public MockIntent {
 public:
  NullIntent() : MockIntent("null", "") {
    // Noop.
  }
};


void TestLoader::registerIntents() {
  NodeConfigIntentLuaProxy type;
  LuaTable test = this->lua.stack()->newTable();
  this->lua.globals()->fromStack("test");

  type.wrap(this->lua, new Cycle1Intent());
  test.fromStack("cycle1");
  type.wrap(this->lua, new Cycle2Intent());
  test.fromStack("cycle2");
  type.wrap(this->lua, new EventManagerIntent());
  test.fromStack("event_manager");
  type.wrap(this->lua, new EventTcpIntent());
  test.fromStack("event_tcp");
  type.wrap(this->lua, new EventUnixIntent());
  test.fromStack("event_unix");
  type.wrap(this->lua, new NullIntent());
  test.fromStack("null");
}

TestLoader::TestLoader(std::string ver) : NodeConfigLoader(ver) {
  this->init_count = 0;
}

ContextRef TestLoader::getContext() {
  return this->new_context;
}

Lua* TestLoader::getLua() {
  return &this->lua;
}

std::deque<NodeConfigIntentRef> TestLoader::getIntents() {
  return this->intents;
}

std::string TestLoader::getEffective() {
  return this->effective;
}

void TestLoader::collectIntents() {
  std::vector<std::string>::iterator it;
  std::vector<std::string> intents = {
    "cycle1",
    "cycle2",
    "event_tcp",
    "event_manager",
    "event_unix",
    "null"
  };

  this->lua.doString("return intents");
  LuaTable intents_table(&this->lua, -1, true);
  NodeConfigIntentLuaProxy type(this->lua);

  for (it = intents.begin(); it != intents.end(); it++) {
    intents_table.toStack(*it);
    if (this->lua.stack()->type() != LUA_TUSERDATA) {
      this->lua.stack()->remove(-1);
      continue;
    }

    NodeConfigIntentRef intent = type.get();
    this->addIntent(intent);
  }
}

void TestLoader::initLua() {
  this->init_count += 1;
  this->lua.doString("core = {}");
  this->lua.doString("connector = {}");
  this->lua.doString("function events_from(src) end");

  this->registerIntents();
  this->lua.doString("intents = {}");
  this->lua.doString("intents.null = test.null");

  std::vector<std::string>::iterator it;
  for (it = this->lines.begin(); it != this->lines.end(); it++) {
    this->lua.doString(*it);
  }
}

void TestLoader::loadToSort() {
  this->initialise();
  this->run();
  this->collectIntents();
  this->sort();
}


NodeConfigLoaderTest::NodeConfigLoaderTest() {
  git_libgit2_init();
  Static::parser(new TestParser());
  Static::repository(RepositoryRef(new GitRepo("config-example/")));
  this->loader = std::shared_ptr<TestLoader>(
      new TestLoader("refs/heads/test-fixture")
  );
}

NodeConfigLoaderTest::~NodeConfigLoaderTest() {
  this->loader = std::shared_ptr<TestLoader>();
  Static::destroy();
  git_libgit2_shutdown();
}


void NodeConfigIntentsOrderTest::ASSERT_ORDER(
    std::vector<std::string> expected
) {
  std::vector<std::string> actual;
  std::deque<NodeConfigIntentRef> intents = this->loader->getIntents();
  std::deque<NodeConfigIntentRef>::iterator it;
  for (it = intents.begin(); it != intents.end(); it++) {
    actual.push_back((*it)->id());
  }

  ASSERT_EQ(expected.size(), actual.size());
  for (int idx = 0; idx < actual.size(); idx++) {
    ASSERT_EQ(expected[idx], actual[idx]);
  }
}

void NodeConfigIntentsOrderTest::useIntent(std::string intent) {
  this->loader->lines.push_back(
      "intents." + intent + " = test." + intent
  );
}
