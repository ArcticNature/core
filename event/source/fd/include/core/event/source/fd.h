// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_SOURCE_FD_H_
#define CORE_EVENT_SOURCE_FD_H_

#include <string>

#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  //! Event source based on readable file descriptors.
  /*!
   * The file descriptor is closed when the source is destroied.
   */
  class FdSource : public sf::core::model::EventSource {
   protected:
    int fd;

   public:
    FdSource(int fd, std::string id);
    ~FdSource();

    int getFD();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_SOURCE_FD_H_
