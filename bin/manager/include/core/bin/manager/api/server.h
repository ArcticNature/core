// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_MANAGER_API_SERVER_H_
#define CORE_BIN_MANAGER_API_SERVER_H_

#include <string>

#include "core/event/source/unix.h"

namespace sf {
namespace core {
namespace bin {

  //! Unix-source for the Public API.
  class UnixServer : public sf::core::event::UnixSource {
   protected:
    sf::core::model::EventDrainRef clientDrain(
        int fd, std::string id
    );
    sf::core::model::EventSourceRef clientSource(
        int fd, std::string id, std::string drain_id
    );

   public:
    explicit UnixServer(std::string socket);
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_MANAGER_API_SERVER_H_
