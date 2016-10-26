// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_MANAGER_EVENT_CONFIG_H_
#define CORE_BIN_MANAGER_EVENT_CONFIG_H_

#include <exception>
#include <string>

#include "core/model/event.h"

namespace sf {
namespace core {
namespace bin {

  //! Reload the manager configuration.
  class LoadConfiguration : public sf::core::model::Event {
   protected:
    bool abort;
    std::string version;

   public:
    LoadConfiguration(
        std::string version,
        sf::core::model::EventDrainRef drain, bool abort = false
    );
    virtual void handle();
    virtual void rescue(std::exception_ptr ex);
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_MANAGER_EVENT_CONFIG_H_
