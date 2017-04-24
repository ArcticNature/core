// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/interface/metadata/store.h"


using nlohmann::json;
using poolqueue::Promise;

using sf::core::exception::NotImplemented;
using sf::core::interface::MetaDataStore;
using sf::core::interface::MetaDataStoreRef;
using sf::core::interface::NoMetadataStore;


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

TEST(MetaDataStore, NoStoreFailsErase) {
  std::exception_ptr fail_reason;
  MetaDataStoreRef store = std::make_shared<NoMetadataStore>();

  auto erase = store->erase("test");
  erase.except([&fail_reason](const std::exception_ptr& ex) {
    fail_reason = ex;
    return nullptr;
  });

  ASSERT_TRUE(erase.settled());
  ASSERT_TRUE(!!fail_reason);
  ASSERT_THROW(std::rethrow_exception(fail_reason), NotImplemented);
}

TEST(MetaDataStore, NoStoreFailsGet) {
  std::exception_ptr fail_reason;
  MetaDataStoreRef store = std::make_shared<NoMetadataStore>();

  auto erase = store->get("test");
  erase.except([&fail_reason](const std::exception_ptr& ex) {
    fail_reason = ex;
    return nullptr;
  });

  ASSERT_TRUE(erase.settled());
  ASSERT_TRUE(!!fail_reason);
  ASSERT_THROW(std::rethrow_exception(fail_reason), NotImplemented);
}

TEST(MetaDataStore, NoStoreFailsSet) {
  std::exception_ptr fail_reason;
  MetaDataStoreRef store = std::make_shared<NoMetadataStore>();

  auto erase = store->set("test", "42"_json);
  erase.except([&fail_reason](const std::exception_ptr& ex) {
    fail_reason = ex;
    return nullptr;
  });

  ASSERT_TRUE(erase.settled());
  ASSERT_TRUE(!!fail_reason);
  ASSERT_THROW(std::rethrow_exception(fail_reason), NotImplemented);
}

TEST(MetaDataStore, NoStoreFailsSetTtl) {
  std::exception_ptr fail_reason;
  MetaDataStoreRef store = std::make_shared<NoMetadataStore>();

  auto erase = store->set("test", "42"_json, std::chrono::duration<int>(1));
  erase.except([&fail_reason](const std::exception_ptr& ex) {
    fail_reason = ex;
    return nullptr;
  });

  ASSERT_TRUE(erase.settled());
  ASSERT_TRUE(!!fail_reason);
  ASSERT_THROW(std::rethrow_exception(fail_reason), NotImplemented);
}
