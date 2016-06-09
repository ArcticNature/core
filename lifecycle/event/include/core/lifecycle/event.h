// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_LIFECYCLE_EVENT_H_
#define CORE_LIFECYCLE_EVENT_H_

#include "core/compile-time/options.h"
#include "core/interface/lifecycle.h"
#include "core/model/event.h"


namespace sf {
namespace core {
namespace lifecycle {

  //! Aliases to event stages.
  class EventLifecycle {
   public:
    //! Initialise an event.
    static void Init(sf::core::model::EventRef event);

    //! Creates an EventRef from an Event subclass and init it.
    template <class EventClass, typename... Args>
    static sf::core::model::EventRef make(Args... args) {
      sf::core::model::EventRef event(new EventClass(args...));
      EventLifecycle::Init(event);
      return event;
    }

#if TEST_BUILD
    static void reset();
#endif
  };

  //! Lifecycle handler argument that stores an EventRef.
  class EventLifecycleArg : public sf::core::interface::BaseLifecycleArg {
   public:
    sf::core::model::EventRef event;
    explicit EventLifecycleArg(sf::core::model::EventRef event);
  };

}  // namespace lifecycle
}  // namespace core
}  // namespace sf

#endif  // CORE_LIFECYCLE_EVENT_H_
