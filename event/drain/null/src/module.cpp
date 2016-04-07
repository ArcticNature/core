// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/context/static.h"
#include "core/event/drain/null.h"
#include "core/interface/lifecycle.h"
#include "core/lifecycle/process.h"


using sf::core::context::Static;
using sf::core::event::NullDrain;
using sf::core::model::EventDrainRef;

using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;
using sf::core::lifecycle::Process;


//! Initialisation handler.
class NullDrainInitHandler : public BaseLifecycleHandler {
 public:
  void handle(std::string event, BaseLifecycleArg*) {
    EventDrainRef drain(new NullDrain());
    Static::drains()->add(drain);
  }
};


// Register the initialiser.
LifecycleStaticOn("process::init", NullDrainInitHandler);
