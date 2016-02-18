// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_DRAIN_FD_H_
#define CORE_EVENT_DRAIN_FD_H_

#include <string>

#include "core/exceptions/base.h"
#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  //! EventDrain backed by an open file descriptor.
  /*!
   * The file descriptor is closed when the drain is destroied.
   */
  class FdDrain : public sf::core::model::EventDrain {
   protected:
    int fd;

   public:
    FdDrain(int fd, std::string id);
    ~FdDrain();

    int getFD();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_DRAIN_FD_H_
