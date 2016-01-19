// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_REGISTRY_EVENT_MANAGERS_H_
#define CORE_REGISTRY_EVENT_MANAGERS_H_

#include "core/model/event.h"
#include "core/registry/base.h"

namespace sf {
namespace core {
namespace registry {

  //! Type of factory functions for event source managers.
  typedef sf::core::model::EventSourceManagerRef (*make_source_manager)();

  //! Type of factory functions for event drain managers.
  typedef sf::core::model::EventDrainManagerRef (*make_drain_manager)();

  //! Registry class for event source managers.
  class EventSourceManager : public Registry<make_source_manager> {};

  //! Registry class for event drain managers.
  class EventDrainManager : public Registry<make_drain_manager> {};

}  // namespace registry
}  // namespace core
}  // namespace sf

#endif  // CORE_REGISTRY_EVENT_MANAGERS_H_
