// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_SOURCE_SCHEDULED_H_
#define CORE_EVENT_SOURCE_SCHEDULED_H_

#include <string>

#include "core/model/event.h"
#include "core/utility/scored-list.h"

namespace sf {
namespace core {
namespace event {

  //! Type for callbacks registered with the ScheduledSource.
  /*!
   * When a service needs to delay execution for a multiple of the
   * tick unit it registers a callback with the ScheduledSource.
   * The callback is executed after the requested amount of ticks
   * are elapsed.
   * 
   * The callback may return an EventRef if it so whishes.
   * If not it must return an empty EventRef.
   */
  typedef sf::core::model::EventRef (*scheduled_callback)(void*);


  //! Structure to hold the callback for a scheduled event.
  struct ScheduledClosure {
    scheduled_callback callback;
    void* closure;
  };


  //! Manages time delayed events.
  class ScheduledSource : public sf::core::model::EventSource {
   protected:
    int tick;
    int timer_fd;

    sf::core::utility::ScoredList<ScheduledClosure> closures;

   public:
    ScheduledSource(int tick, std::string id);
    ~ScheduledSource();

    void registerCallback(unsigned int score, ScheduledClosure closure);
    void registerCallback(unsigned int score, scheduled_callback callback);

    int getFD();
    sf::core::model::EventRef parse();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_SOURCE_SCHEDULED_H_
