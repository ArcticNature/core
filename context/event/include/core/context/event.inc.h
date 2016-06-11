// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONTEXT_EVENT_INC_H_
#define CORE_CONTEXT_EVENT_INC_H_

#include <map>
#include <string>

namespace sf {
namespace core {
namespace context {

  // Define the templated instance variable.
  template<typename Context>
  EventContext<Context>* EventContext<Context>::_instance;

  // Singleton instance accessor.
  template<typename Context>
  EventContext<Context>* EventContext<Context>::instance() {
    if (EventContext<Context>::_instance == nullptr) {
      EventContext<Context>::_instance = new EventContext<Context>();
    }
    return EventContext<Context>::_instance;
  }


  // *** Public static methods. ***
  template<typename Context>
  void EventContext<Context>::destroy() {
    if (EventContext<Context>::_instance == nullptr) {
      return;
    }
    delete EventContext<Context>::_instance;
    EventContext<Context>::_instance = nullptr;
  }

  template<typename Context>
  Context EventContext<Context>::pop(std::string correlation) {
    EventContext<Context>* instance = EventContext<Context>::instance();
    if (instance->contexts.find(correlation) == instance->contexts.end()) {
      throw sf::core::exception::ContextUninitialised(
          "No context stored for events with correlation " +
          correlation
      );
    }
    typename std::map<std::string, Context>::iterator it;
    it = instance->contexts.find(correlation);
    Context context = it->second;
    instance->contexts.erase(it);
    return context;
  }

  template<typename Context>
  void EventContext<Context>::store(
      std::string correlation, Context context
  ) {
    EventContext<Context>* instance = EventContext<Context>::instance();
    if (instance->contexts.find(correlation) != instance->contexts.end()) {
      throw sf::core::exception::DuplicateItem(
          "A context for correlation '" + correlation +
          "' already exists."
      );
    }
    EventContext<Context>::instance()->contexts.insert(
        std::make_pair(correlation, context)
    );
  }

  // *** Instance methods. ***
  template<typename Context>
  EventContext<Context>::EventContext() {
    this->handler = sf::core::interface::LifecycleHandlerRef(
        new EventContextCleanupHandler(&EventContext<Context>::destroy)
    );
    sf::core::interface::Lifecycle::on("process::exit", this->handler);
  }

  template<typename Context>
  EventContext<Context>::~EventContext() {
    sf::core::interface::Lifecycle::off("process::exit", this->handler);
  }

}  // namespace context
}  // namespace core
}  // namespace sf

#endif  // CORE_CONTEXT_EVENT_INC_H_
