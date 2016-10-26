// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_MANAGER_API_SOURCE_H_
#define CORE_BIN_MANAGER_API_SOURCE_H_

#include <string>

#include "core/event/source/fd.h"

namespace sf {
namespace core {
namespace bin {

  //! Server-side event source for the public API.
  class ApiFdSource : public sf::core::event::FdSource {
   protected:
    sf::core::model::EventRef parse();

   public:
    ApiFdSource(int fd, std::string id, sf::core::model::EventDrainRef drain);
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_MANAGER_API_SOURCE_H_
