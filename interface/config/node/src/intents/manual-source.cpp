// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/config/node/intents/manual-source.h"

#include <string>
#include <vector>

#include "core/context/context.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::interface::CopyManualSourceIntent;


CopyManualSourceIntent::CopyManualSourceIntent() : NodeConfigIntent(
    "event.source.manual"
) {
  this->dependsList.push_back("event.manager");
}

void CopyManualSourceIntent::apply(ContextRef context) {
  context->loopManager()->add(Context::LoopManager()->fetch("manual"));
}

std::vector<std::string> CopyManualSourceIntent::depends() const {
  return this->dependsList;
}

std::string CopyManualSourceIntent::provides() const {
  return "event.source.manual";
}

void CopyManualSourceIntent::verify(ContextRef context) {
  Context::LoopManager()->fetch("manual");
}
