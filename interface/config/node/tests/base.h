// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef BASE_H_
#define BASE_H_

#include <gtest/gtest.h>

#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/interface/config/node.h"

namespace sf {
namespace core {
namespace test {

  //! Intent that does nothing but can be used in tests.
  class MockIntent : public sf::core::interface::NodeConfigIntent {
   protected:
    std::string _provides;

   public:
    MockIntent(std::string id, std::string provides);

    std::vector<std::string> depends() const;
    std::string provides() const;
    void apply(sf::core::context::ContextRef context);
    void verify(sf::core::context::ContextRef context);
  };

  //! Node config loaded to test on.
  class TestLoader : public sf::core::interface::NodeConfigLoader {
   protected:
    static const std::vector<std::string> PATHS;
    void registerIntents();

   public:
    int init_count;
    std::vector<std::string> lines;

    TestLoader(std::string ver);
    sf::core::context::ContextRef getContext();
    sf::core::utility::Lua* getLua();
    std::deque<sf::core::interface::NodeConfigIntentRef> getIntents();
    std::string getEffective();

    void collectIntents();
    void initLua();

    void loadToSort();
  };


  class TestIntentLoader : public sf::core::interface::NodeConfigLoader {
   protected:
    std::vector<sf::core::interface::NodeConfigIntentRef> mocks;

   public:
    TestIntentLoader();
    void addIntent(sf::core::interface::NodeConfigIntentRef intent);
    void collectIntents();

    sf::core::utility::Lua* getLua();
    void initLua();

    std::vector<sf::core::interface::NodeConfigIntentRef> getMocks();
  };


  //! Test case for node configuraation environment.
  class NodeConfigEventsFrom : public ::testing::Test {
   protected:
    std::shared_ptr<TestIntentLoader> loader;

    NodeConfigEventsFrom();
    ~NodeConfigEventsFrom();
  };

  //! Test case for node configuration loader tests.
  class NodeConfigLoaderTest : public ::testing::Test {
   protected:
    std::shared_ptr<TestLoader> loader;

    NodeConfigLoaderTest();
    ~NodeConfigLoaderTest();
  };

  //! Test case for node configuration intent tests.
  class NodeConfigIntentTest : public ::testing::Test {
   protected:
    std::shared_ptr<TestIntentLoader> loader;

    NodeConfigIntentTest();
    ~NodeConfigIntentTest();

    void addIntent(sf::core::interface::NodeConfigIntentRef intent);
    void load();
  };

  class NodeConfigIntentsOrderTest : public NodeConfigLoaderTest {
   protected:
    void ASSERT_ORDER(std::vector<std::string> expected);
    void useIntent(std::string intent);
  };

}  // namespace test
}  // namespace core
}  // namespace sf

#endif  // BASE_H_
