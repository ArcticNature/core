// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <poolqueue/Promise.hpp>

#include <memory>
#include <string>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/source/manual.h"
#include "core/interface/lifecycle.h"
#include "core/model/event.h"


using poolqueue::Promise;

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::ManualSource;
using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;

using sf::core::lifecycle::EVENT_DRAIN_ENQUEUE;
using sf::core::lifecycle::DrainEnqueueArg;
using sf::core::lifecycle::FlushEventDrain;

using sf::core::model::EventDrain;
using sf::core::model::EventDrainBufferRef;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::ReThrowEvent;


class NullDrain : public EventDrain {
 public:
  NullDrain() : EventDrain("PromiseErrorDrain") {
    // Noop.
  }

  int fd() {
    return -1;
  }

  virtual void enqueue(EventDrainBufferRef chunk) {
    return;
  }

  bool flush() {
    return true;
  }
};


static bool intercept_promises = false;
static Promise::ExceptionHandler original_promise_handler;


void sf::core::context::CatchPromises() {
  auto undelivered = [](const std::exception_ptr& ex) {
    // Create a new ReThrowEvent.
    EventRef event = std::make_shared<ReThrowEvent>(
        ex, "global_promise_handler",
        std::make_shared<NullDrain>()
    );

    // Add it to the ManualSource.
    ManualSource* manual = Context::LoopManager()->downcast<ManualSource>(
        "manual"
    );
    manual->enqueueEvent(event);
    return;
  };

  original_promise_handler = \
    Promise::setUndeliveredExceptionHandler(undelivered);
  intercept_promises = true;
}

void sf::core::context::ResetPromiseHandler() {
  if (!intercept_promises) {
    return;
  }
  Promise::setUndeliveredExceptionHandler(original_promise_handler);
  intercept_promises = false;
}


void FlushEventDrain::handle(std::string event, DrainEnqueueArg* argument) {
  EventDrainRef drain = Static::drains()->get(argument->drain());
  Context::LoopManager()->add(drain);
  argument->add();
}


class ContextModuleInit : public BaseLifecycleHandler {
 public:
  void handle(std::string event, BaseLifecycleArg* arg) {
    sf::core::context::CatchPromises();
  }
};


class ContextModuleExit : public BaseLifecycleHandler {
 public:
  void handle(std::string event, BaseLifecycleArg* arg) {
    sf::core::context::ResetPromiseHandler();
  }
};


LifecycleStaticOn(EVENT_DRAIN_ENQUEUE, FlushEventDrain);
LifecycleStaticOn("process::init", ContextModuleInit);
LifecycleStaticOn("process::exit", ContextModuleExit);
