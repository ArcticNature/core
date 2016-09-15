// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_INTERFACE_CONFIG_NODE_INTENTS_MANUAL_SOURCE_H_
#define CORE_INTERFACE_CONFIG_NODE_INTENTS_MANUAL_SOURCE_H_

#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/interface/config/node.h"

namespace sf {
namespace core {
namespace interface {

  //! Ensure that the ManualSource is copied from the current context.
  class CopyManualSourceIntent : public NodeConfigIntent {
   protected:
    std::vector<std::string> dependsList;

   public:
    CopyManualSourceIntent();

    void apply(sf::core::context::ContextRef context);
    std::vector<std::string> depends() const;
    std::string provides() const;
    void verify(sf::core::context::ContextRef context);
  };

}  // namespace interface
}  // namespace core
}  // namespace sf

#endif  // CORE_INTERFACE_CONFIG_NODE_INTENTS_MANUAL_SOURCE_H_
