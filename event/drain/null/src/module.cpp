// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/context/static.h"
#include "core/event/drain/null.h"
#include "core/interface/lifecycle.h"


using sf::core::context::Static;
using sf::core::event::NullDrain;
using sf::core::model::EventDrainRef;

using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;
using sf::core::interface::Lifecycle;
using sf::core::interface::LifecycleHandlerRef;


//! Initialisation handler.
class NullDrainInitHandler : public BaseLifecycleHandler {
 public:
  void handle(std::string event, BaseLifecycleArg*) {
    EventDrainRef drain(new NullDrain());
    Static::drains()->add(drain);
  }
};


//! Module initialiser.
class NullDrainModuleInit {
 public:
  NullDrainModuleInit() {
    LifecycleHandlerRef handler(new NullDrainInitHandler());
    Lifecycle::on("process::init", handler);
  }
};
NullDrainModuleInit null_drain_module;
