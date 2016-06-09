// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_CLIENT_EVENTS_H_
#define CORE_BIN_CLIENT_EVENTS_H_

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

  //! Node status event context.
  typedef sf::core::context::EventContext<NSClientContext> 
    NodeStatusContext;

  //! Request the state of a node.
  class NodeStatusRequest : public sf::core::model::Event {
   protected:
    int  callback_ref;
    bool details;

   public:
    NodeStatusRequest(
        int callback_ref, bool details, std::string drain
    );
    void handle();
    void rescue(sf::core::exception::SfException* ex);
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_CLIENT_EVENTS_H_
