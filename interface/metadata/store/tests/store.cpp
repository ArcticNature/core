// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/interface/metadata/store.h"


using nlohmann::json;
using poolqueue::Promise;

using sf::core::interface::MetaDataStore;
using sf::core::interface::MetaDataStoreRef;


class TestStore : public MetaDataStore {
 public:
  Promise erase(std::string key) {
    return Promise();
  }

  Promise get(std::string key) {
    return Promise();
  }

  Promise set(std::string key, json value) {
    return Promise();
  }

  Promise set(std::string key, json value, std::chrono::duration<int> ttl) {
    return Promise();
  }
};


TEST(MetaDataStore, SelfReferences) {
  MetaDataStoreRef store = std::make_shared<TestStore>();
  auto new_ref = store->shared_from_this();
  ASSERT_EQ(2, store.use_count());
}
