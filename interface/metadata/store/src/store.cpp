// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/metadata/store.h"

#include <string>
#include "core/exceptions/base.h"


using nlohmann::json;
using poolqueue::Promise;

using sf::core::exception::NotImplemented;
using sf::core::interface::NoMetadataStore;


Promise NoMetadataStore::erase(std::string key) {
  return Promise().settle().then([]() {
    throw NotImplemented("NoMetadataStore::erase is not implemented");
    return nullptr;
  });
}

Promise NoMetadataStore::get(std::string key) {
  return Promise().settle().then([]() {
    throw NotImplemented("NoMetadataStore::get is not implemented");
    return nullptr;
  });
}

Promise NoMetadataStore::set(std::string key, json value) {
  return Promise().settle().then([]() {
    throw NotImplemented("NoMetadataStore::set is not implemented");
    return nullptr;
  });
}

Promise NoMetadataStore::set(
    std::string key, json value, std::chrono::duration<int> ttl
) {
  return Promise().settle().then([]() {
    throw NotImplemented("NoMetadataStore::set is not implemented");
    return nullptr;
  });
}
