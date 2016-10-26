// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LOOKUP_TABLE_INC_H_
#define CORE_UTILITY_LOOKUP_TABLE_INC_H_

#include <map>
#include <string>
#include <utility>

namespace sf {
namespace core {
namespace utility {

  template<typename Value, typename DuplicateKey, typename NotFound>
  void LookupTable<Value, DuplicateKey, NotFound>::add(
      std::string key, Value value
  ) {
    if (this->find(key) != this->end()) {
      throw DuplicateKey(key);
    }
    this->insert(std::pair<std::string, Value>(key, value));
  }

  template<typename Value, typename DuplicateKey, typename NotFound>
  Value LookupTable<Value, DuplicateKey, NotFound>::get(
      std::string key
  ) const {
    typename std::map<std::string, Value>::const_iterator it;
    it = this->find(key);
    if (it == this->end()) {
      throw NotFound(key);
    }
    return it->second;
  }

  template<typename Value, typename DuplicateKey, typename NotFound>
  void LookupTable<Value, DuplicateKey, NotFound>::remove(
      std::string key
  ) {
    if (this->find(key) == this->end()) {
      throw NotFound(key);
    }
    this->erase(key);
  }

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_LOOKUP_TABLE_INC_H_
