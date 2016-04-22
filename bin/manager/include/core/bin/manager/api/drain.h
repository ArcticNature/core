// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_MANAGER_API_DRAIN_H_
#define CORE_BIN_MANAGER_API_DRAIN_H_

#include <string>

#include "core/event/drain/fd.h"

namespace sf {
namespace core {
namespace bin {

  //! Server-side event drain for the public API.
  class ApiFdDrain : public sf::core::event::FdDrain {
   public:
    ApiFdDrain(int fd, std::string id);
    void sendAck();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_MANAGER_API_DRAIN_H_
