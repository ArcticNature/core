// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_CLIENT_EVENTS_H_
#define CORE_BIN_CLIENT_EVENTS_H_

#include <memory>
#include <string>

#include "core/context/event.h"
#include "core/model/event.h"

namespace sf {
namespace core {
namespace bin {

  //! Data container for contexts of node status requests.
  class NSClientContext {
   public:
    int callback_ref;

    NSClientContext();
    ~NSClientContext();
  };
  typedef std::shared_ptr<NSClientContext> NSClientContextRef;

  //! Node status event context.
  typedef sf::core::context::EventContext<NSClientContextRef>
    NodeStatusContext;

  //! Request the state of a node.
  class NodeStatusRequest : public sf::core::model::Event {
   protected:
    int  callback_ref;
    bool details;

   public:
    NodeStatusRequest(
        int callback_ref, bool details,
        sf::core::model::EventDrainRef drain
    );
    void handle();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_CLIENT_EVENTS_H_
