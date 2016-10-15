// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/config/node/intents/scheduled-source.h"

#include <unistd.h>
#include <memory>

#include "core/context/context.h"
#include "core/event/source/manual.h"
#include "core/event/source/scheduled.h"
#include "core/event/testing.h"
#include "core/exceptions/event.h"

#include "core/interface/config/node.h"
#include "core/model/event.h"

#include "../base.h"

using sf::core::context::Context;
using sf::core::event::ManualSource;
using sf::core::event::ScheduledClosure;
using sf::core::event::ScheduledSource;
using sf::core::event::TestEvent;
using sf::core::exception::EventSourceNotFound;

using sf::core::interface::DefaultScheduledSourceIntent;
using sf::core::interface::NodeConfigIntentRef;

using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;

using sf::core::test::NodeConfigIntentTest;


EventRef mock_event(std::shared_ptr<void> closure) {
  return EventRef(new TestEvent());
}


TEST_F(NodeConfigIntentTest, ScheduledSourceIsCreated) {
  ASSERT_THROW(
      Context::LoopManager()->fetch("scheduler"),
      EventSourceNotFound
  );
  this->addIntent(NodeConfigIntentRef(new DefaultScheduledSourceIntent()));
  this->load();
  Context::LoopManager()->fetch("scheduler");
}

TEST_F(NodeConfigIntentTest, ScheduledSourceIsCreatedAndPopulated) {
  EventSourceRef source(new ScheduledSource(4));
  Context::LoopManager()->add(source);
  ScheduledSource* scheduler = Context::LoopManager()->get<
      ScheduledSource
  >("scheduler");

  ScheduledClosure closure;
  closure.keep_on_reconfigure = true;
  closure.callback = mock_event;
  scheduler->registerCallback(0, closure);

  this->addIntent(NodeConfigIntentRef(new DefaultScheduledSourceIntent()));
  this->load();

  EventSourceRef manual(new ManualSource());
  Context::LoopManager()->fetch("scheduler");
  Context::LoopManager()->add(manual);

  sleep(2);  // Seconds.
  EventRef event = Context::LoopManager()->wait(1);
  ASSERT_NE(nullptr, event.get());
}
