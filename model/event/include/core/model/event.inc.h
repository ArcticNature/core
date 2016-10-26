// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_EVENT_INC_H_
#define CORE_MODEL_EVENT_INC_H_

#include <map>
#include <string>
#include <utility>

#include "core/exceptions/base.h"
#include "core/utility/string.h"

namespace sf {
namespace core {
namespace model {

  template<typename Ref>
  void RefStore<Ref>::add(Ref ref) {
    int fd = ref->fd();
    std::string id = ref->id();

    bool have_fd = this->by_fd.find(fd) != this->by_fd.end();
    bool have_id = this->by_id.find(id) != this->by_id.end();
    if (have_fd || have_id) {
      throw sf::core::exception::DuplicateItem(
          "RefStore already has an item with id '" + id +"'"
      );
    }

    this->by_id.insert(std::make_pair(id, ref));
    this->by_fd.insert(std::make_pair(fd, ref));
  }

  template<typename Ref>
  Ref RefStore<Ref>::get(int fd) const {
    typename std::map<int, Ref>::const_iterator it;
    it = this->by_fd.find(fd);
    if (it == this->by_fd.end()) {
      throw sf::core::exception::ItemNotFound(
          "RefStore does not have an item with fd '" +
          sf::core::utility::string::toString(fd) +
          "'."
      );
    }
    return it->second;
  }

  template<typename Ref>
  Ref RefStore<Ref>::get(std::string id) const {
    typename std::map<std::string, Ref>::const_iterator it;
    it = this->by_id.find(id);
    if (it == this->by_id.end()) {
      throw sf::core::exception::ItemNotFound(
          "RefStore does not have an item with id '" + id + "'."
      );
    }
    return it->second;
  }

  template<typename Ref>
  void RefStore<Ref>::remove(int fd) {
    std::string id = this->get(fd)->id();
    this->by_id.erase(id);
    this->by_fd.erase(fd);
  }

  template<typename Ref>
  void RefStore<Ref>::remove(std::string id) {
    int fd = this->get(id)->fd();
    this->by_id.erase(id);
    this->by_fd.erase(fd);
  }

  template<typename EventSrc>
  EventSrc* LoopManager::downcast(std::string id) const {
    EventSourceRef ref = this->fetch(id);
    EventSrc* source = dynamic_cast<EventSrc*>(ref.get());
    if (source == nullptr) {
      throw sf::core::exception::IncorrectSourceType(id);
    }
    return source;
  }

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_EVENT_INC_H_
