// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_TESTING_METADATA_H_
#define CORE_TESTING_METADATA_H_

#include <json.hpp>
#include <poolqueue/Promise.hpp>

#include <chrono>  // NOLINT(build/c++11)
#include <map>
#include <string>

#include "core/interface/metadata/store.h"


namespace sf {
namespace core {
namespace testing {

  class TestMetaStore : public sf::core::interface::MetaDataStore {
   protected:
    std::map<std::string, nlohmann::json> store_;

   public:
    poolqueue::Promise erase(std::string key);
    poolqueue::Promise get(std::string key);
    poolqueue::Promise set(std::string key, nlohmann::json value);
    poolqueue::Promise set(
        std::string key, nlohmann::json value,
        std::chrono::duration<int> ttl
    );
  };

}  // namespace testing
}  // namespace core
}  // namespace sf

#endif  // CORE_TESTING_METADATA_H_
