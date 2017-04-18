// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/promise.h"

#include <poolqueue/Promise.hpp>

#include <map>
#include <string>
#include <vector>

#include "core/exceptions/base.h"


using poolqueue::Promise;

using sf::core::PromiseKeeper;
using sf::core::exception::DuplicateItem;
using sf::core::exception::ItemNotFound;
using sf::core::exception::TimeoutError;


void PromiseKeeper::clear() {
  std::vector<std::string> ids;
  for (auto pair : this->promises_) {
    ids.push_back(pair.first);
  }
  for (auto id : ids) {
    this->pop(id);
  }
}

void PromiseKeeper::keep(std::string id, Promise promise, unsigned int ttl) {
  if (this->promises_.find(id) != this->promises_.end()) {
    throw DuplicateItem("Can't keep two '" + id + "' promises");
  }

  PromiseKeeper::PMetaData meta;
  meta.expire = ttl != 0;
  meta.promise = promise;
  meta.ttl = ttl;
  this->promises_.insert(std::make_pair(id, meta));
}

Promise PromiseKeeper::pop(std::string id) {
  auto iter = this->promises_.find(id);
  if (iter == this->promises_.end()) {
    throw ItemNotFound("Can't find promise '" + id + "'");
  }

  auto meta = iter->second;
  this->promises_.erase(iter);
  return meta.promise;
}

void PromiseKeeper::tick() {
  std::vector<std::string> timed_out;

  // Tick all expiring promises.
  for (auto it = this->promises_.begin(); it != this->promises_.end(); it++) {
    PromiseKeeper::PMetaData* meta = &it->second;
    if (meta->expire && meta->ttl > 0) {
      meta->ttl -= 1;
    }
    if (meta->expire && meta->ttl == 0) {
      timed_out.push_back(it->first);
    }
  }

  // Remove and reject all promiese.
  for (auto id : timed_out) {
    auto meta = this->promises_.at(id);
    auto promise = meta.promise;
    promise.settle(std::make_exception_ptr(TimeoutError()));
    this->promises_.erase(id);
  }
}
