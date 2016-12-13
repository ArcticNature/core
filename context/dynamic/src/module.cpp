// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/interface/lifecycle.h"
#include "core/model/event.h"

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::lifecycle::EVENT_DRAIN_ENQUEUE;
using sf::core::lifecycle::DrainEnqueueArg;
using sf::core::lifecycle::FlushEventDrain;

using sf::core::model::EventDrainRef;


void FlushEventDrain::handle(std::string event, DrainEnqueueArg* argument) {
  EventDrainRef drain = Static::drains()->get(argument->drain());
  Context::LoopManager()->add(drain);
  argument->add();
}


LifecycleStaticOn(EVENT_DRAIN_ENQUEUE, FlushEventDrain);
