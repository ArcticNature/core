// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_REGISTRY_EVENT_SOURCES_H_
#define CORE_REGISTRY_EVENT_SOURCES_H_

#include "core/model/event.h"
#include "core/registry/base.h"

namespace sf {
namespace core {
namespace registry {

  //! Type of factory functions for event sources.
  typedef sf::core::model::EventSourceRef (*make_source)();

  //! Registry class for event sources.
  class EventSource : public Registry<make_source> {};

}  // namespace registry
}  // namespace core
}  // namespace sf

#endif  // CORE_REGISTRY_EVENT_SOURCES_H_
