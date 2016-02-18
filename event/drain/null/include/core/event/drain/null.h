// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_DRAIN_NULL_H_
#define CORE_EVENT_DRAIN_NULL_H_

#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  //! Blackhole event drain.
  class NullDrain : public sf::core::model::EventDrain {
   protected:
    int null_fd;

   public:
    NullDrain();
    ~NullDrain();

    int getFD();
    void sendAck();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_DRAIN_NULL_H_
