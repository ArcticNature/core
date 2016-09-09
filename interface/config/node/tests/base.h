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


  //! Test case for node configuration loader tests.
  class NodeConfigLoaderTest : public ::testing::Test {
   protected:
    std::shared_ptr<TestLoader> loader;

    NodeConfigLoaderTest();
    ~NodeConfigLoaderTest();
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
