// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/config/node/intents/scheduled-source.h"

#include <string>
#include <vector>

#include "core/context/context.h"

#include "core/event/source/scheduled.h"
#include "core/exceptions/event.h"

#include "core/model/event.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;

using sf::core::event::ScheduledClosureList;
using sf::core::event::ScheduledSource;
using sf::core::exception::EventSourceNotFound;

using sf::core::interface::DefaultScheduledSourceIntent;
using sf::core::interface::ScheduledSourceIntent;
using sf::core::model::EventSourceRef;


const std::vector<std::string> DefaultScheduledSourceIntent::AFTER = {
  "event.source.scheduled"
};

const std::vector<std::string> DefaultScheduledSourceIntent::DEPS = {
  "event.manager"
};

const std::vector<std::string> ScheduledSourceIntent::DEPS = {
  "event.manager"
};


DefaultScheduledSourceIntent::DefaultScheduledSourceIntent() :
    NodeConfigIntent("event.source.scheduled.default") {
  // NOOP.
}

std::vector<std::string> DefaultScheduledSourceIntent::after() const {
  return DefaultScheduledSourceIntent::AFTER;
}

std::vector<std::string> DefaultScheduledSourceIntent::depends() const {
  return DefaultScheduledSourceIntent::DEPS;
}

std::string DefaultScheduledSourceIntent::provides() const {
  return "event.source.scheduled.default";
}

void DefaultScheduledSourceIntent::apply(ContextRef context) {
  // Create a ScheduledSource if none is in the context.
  try {
    context->instanceSourceManager()->fetch("scheduler");
  } catch (EventSourceNotFound& ex) {
    context->instanceSourceManager()->add(EventSourceRef(
        new ScheduledSource(1)
    ));
  }

  // Copy all the callbacks to keep (if a ScheduledSource exists).
  std::vector<ScheduledClosureList::ScoredValue> callbacks;
  std::vector<ScheduledClosureList::ScoredValue>::iterator it;

  try {
    ScheduledSource* old_scheduler = Context::sourceManager()->get<
      ScheduledSource
    >("scheduler");
    callbacks = old_scheduler->keepCallbacks();
  } catch (EventSourceNotFound& ex) {
    // NOOP.
  }

  ScheduledSource* new_scheduler =
    context->instanceSourceManager()->get<ScheduledSource>("scheduler");

  for (it = callbacks.begin(); it != callbacks.end(); it++) {
    new_scheduler->registerCallback(it->score, it->value);
  }
}

void DefaultScheduledSourceIntent::verify(ContextRef context) {
  // NOOP.
}


ScheduledSourceIntent::ScheduledSourceIntent(
    int tick
) : NodeConfigIntent("event.source.scheduled") {
  this->tick = tick;
}

std::vector<std::string> ScheduledSourceIntent::depends() const {
  return ScheduledSourceIntent::DEPS;
}

std::string ScheduledSourceIntent::provides() const {
  return "event.source.scheduled";
}

void ScheduledSourceIntent::apply(ContextRef context) {
  context->instanceSourceManager()->add(EventSourceRef(
      new ScheduledSource(this->tick)
  ));
}

void ScheduledSourceIntent::verify(ContextRef context) {
  // Noop.
}
