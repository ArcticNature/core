// Copyright 2016 Stefano Pogliani
#ifndef CORE_REGISTRY_EVENT_HANDLER_API_H_
#define CORE_REGISTRY_EVENT_HANDLER_API_H_

#include <string>

#include "core/model/event.h"
#include "core/protocols/public/p_message.pb.h"
#include "core/registry/base.h"

namespace sf {
namespace core {
namespace registry {

  //! Signature of factory functions to create events.
  typedef sf::core::model::EventRef (*ApiEventFactory)(
      sf::core::protocol::public_api::Message message,
      sf::core::model::EventDrainRef drain
  );

  //! Factory registry for public API message parsing.
  typedef Registry<ApiEventFactory> ApiHandlerRegistry;

}  // namespace registry
}  // namespace core
}  // namespace sf

#endif  // CORE_REGISTRY_EVENT_HANDLER_API_H_
