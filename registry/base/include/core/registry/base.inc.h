// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_REGISTRY_BASE_INC_H_
#define CORE_REGISTRY_BASE_INC_H_

#include <memory>
#include <string>

#include "core/exceptions/base.h"


namespace sf {
namespace core {
namespace registry {

  template<typename factory>
  std::shared_ptr<Registry<factory>> Registry<factory>::singleton_instance;

  template<typename factory>
  Registry<factory>* Registry<factory>::instance() {
    if (!Registry<factory>::singleton_instance) {
      std::shared_ptr<Registry<factory>> instance(new Registry<factory>);
      Registry<factory>::singleton_instance = instance;
    }
    return Registry<factory>::singleton_instance.get();
  }

  template<typename factory>
  factory Registry<factory>::Get(std::string name) {
    return Registry<factory>::instance()->get(name);
  }

  template<typename factory>
  bool Registry<factory>::RegisterFactory(std::string name, factory callback) {
    return Registry<factory>::instance()->registerFactory(name, callback);
  }

#if TEST_BUILD
  template<typename factory>
  void Registry<factory>::destroy() {
    Registry<factory>::singleton_instance = \
        std::shared_ptr<Registry<factory>>();
  }
#endif


  template<typename factory>
  Registry<factory>::~Registry() {}

  template<typename factory>
  factory Registry<factory>::get(std::string name) {
    if (this->factories.find(name) == this->factories.end()) {
      throw sf::core::exception::FactoryNotFound(
          "Could not find factory for type '" + name + "'."
      );
    }
    return this->factories[name];
  }

  template<typename factory>
  bool Registry<factory>::registerFactory(std::string name, factory callback) {
    if (this->factories.find(name) != this->factories.end()) {
      throw sf::core::exception::DuplicateInjection(
          "The registry already contains a '" + name + "'."
      );
    }
    this->factories[name] = callback;
  }

}  // namespace registry
}  // namespace core
}  // namespace sf

#endif  // CORE_REGISTRY_BASE_INC_H_
