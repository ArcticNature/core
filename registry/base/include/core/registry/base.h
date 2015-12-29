// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_REGISTRY_BASE_H_
#define CORE_REGISTRY_BASE_H_

#include <map>
#include <string>


namespace sf {
namespace core {
namespace registry {

  //! Template class for implementation registries.
  template<typename factory>
  class Registry {
   protected:
    static Registry<factory> singleton_instance;

   public:
    static Registry<factory>* instance();

    //! Proxay get method to the singleton instace.
    static factory Get(std::string name);

    //! Proxay registerFactory method to the singleton instace.
    static void RegisterFactory(std::string name, factory callback);

   protected:
    std::map<std::string, factory> factories;

   public:
    virtual ~Registry();

    //! Returns a factory method for the given name.
    factory get(std::string name);

    //! Add a new factory to the registry.
    void registerFactory(std::string name, factory callback);
  };

}  // namespace registry
}  // namespace core
}  // namespace sf


#include "core/registry/base.inc.h"

#endif  // CORE_REGISTRY_BASE_H_
