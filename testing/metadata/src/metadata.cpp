// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/testing/metadata.h"

#include <string>

#include "core/exceptions/base.h"


using nlohmann::json;
using poolqueue::Promise;

using sf::core::exception::ItemNotFound;
using sf::core::testing::TestMetaStore;


Promise TestMetaStore::erase(std::string key) {
  return Promise().settle().then([this, &key](){
    this->store_.erase(key);
    return nullptr;
  });
}

Promise TestMetaStore::get(std::string key) {
  return Promise().settle().then([this, &key]() {
    if (this->store_.find(key) == this->store_.end()) {
      throw ItemNotFound(key);
    }
    return this->store_.at(key);
  });
}

Promise TestMetaStore::set(std::string key, json value) {
  return Promise().settle().then([this, &key, &value]() {
    this->store_[key] = value;
    return nullptr;
  });
}

Promise TestMetaStore::set(
    std::string key, json value,
    std::chrono::duration<int> ttl
) {
  return this->set(key, value);
}
