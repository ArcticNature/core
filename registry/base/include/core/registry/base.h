// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_REGISTRY_BASE_H_
#define CORE_REGISTRY_BASE_H_

#include <map>
#include <memory>
#include <string>

#include "core/compile-time/options.h"


namespace sf {
namespace core {
namespace registry {

  //! Template class for implementation registries.
  template<typename factory>
  class Registry {
   protected:
    static std::shared_ptr<Registry<factory>> singleton_instance;

   public:
    static Registry<factory>* instance();

    //! Proxay get method to the singleton instace.
    static factory Get(std::string name);

    //! Proxy registerFactory method to the singleton instace.
    static bool RegisterFactory(std::string name, factory callback);

#if TEST_BUILD
    static void destroy();
#endif

   protected:
    std::map<std::string, factory> factories;

   public:
    virtual ~Registry();

    //! Returns a factory method for the given name.
    factory get(std::string name);

    //! Add a new factory to the registry.
    bool registerFactory(std::string name, factory callback);
  };

}  // namespace registry
}  // namespace core
}  // namespace sf


#include "core/registry/base.inc.h"


// Helper macros to make RegisterFactory method-like.
#define RF_UNIQUE_2(handler, line) __init_##handler##_##line##__
#define RF_UNIQUE(handler, line)   RF_UNIQUE_2(handler, line)

#define StaticFactory(Registry, name, Handler) \
  static bool RF_UNIQUE(Handler, __LINE__) = \
    Registry::RegisterFactory(name, Handler);

#endif  // CORE_REGISTRY_BASE_H_
