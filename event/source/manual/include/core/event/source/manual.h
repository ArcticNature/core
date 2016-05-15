// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_SOURCE_MANUAL_H_
#define CORE_EVENT_SOURCE_MANUAL_H_

#include <queue>

#include "core/model/event.h"

namespace sf {
namespace core {
namespace event {

  //! EventSource for manually enqueued events.
  class ManualSource : public sf::core::model::EventSource {
   protected:
    std::queue<sf::core::model::EventRef> internal_event_queue;
    int internal_queue_fd;

   public:
    ManualSource();
    ~ManualSource();

    int getFD();
    sf::core::model::EventRef parse();

    //! Appends a new event to the queue of events to process.
    void enqueueEvent(sf::core::model::EventRef event);
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_SOURCE_MANUAL_H_
