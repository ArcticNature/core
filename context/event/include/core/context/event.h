// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONTEXT_EVENT_H_
#define CORE_CONTEXT_EVENT_H_

#include <map>
#include <string>

#include "core/exceptions/base.h"
#include "core/interface/lifecycle.h"

namespace sf {
namespace core {
namespace context {

  //! Template class to share context between events.
  template<typename Context>
  class EventContext {
   protected:
    std::map<std::string, Context> contexts;
    sf::core::interface::LifecycleHandlerRef handler;

    EventContext<Context>();
    ~EventContext<Context>();

    static EventContext<Context>* _instance;
    static EventContext<Context>* instance();

   public:
    //! Clean up all the stored contexts.
    static void destroy();

    //! Retrive a context for a correlation.
    static Context pop(std::string correlation);

    //! Store a context for a group of events.
    static void store(std::string correlation, Context context);
  };


  //! Deals with cleanup of event contexts at process exit.
  class EventContextCleanupHandler :
      public sf::core::interface::BaseLifecycleHandler {
   protected:
    void (*deleter)();

   public:
    explicit EventContextCleanupHandler(void (*deleter)());
    void handle(
        std::string event,
        sf::core::interface::BaseLifecycleArg* argument
    );
  };

}  // namespace context
}  // namespace core
}  // namespace sf

#include "core/context/event.inc.h"

#endif  // CORE_CONTEXT_EVENT_H_
