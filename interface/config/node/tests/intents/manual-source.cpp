// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/config/node/intents/manual-source.h"

#include "core/context/context.h"
#include "core/event/source/manual.h"
#include "core/model/event.h"

#include "../base.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::event::ManualSource;

using sf::core::interface::CopyManualSourceIntent;
using sf::core::interface::NodeConfigIntentRef;

using sf::core::model::EventSourceManagerRef;
using sf::core::test::NodeConfigIntentTest;


TEST_F(NodeConfigIntentTest, EnsureManualSourceIsCarried) {
  ContextRef old_context = Context::instance();
  ManualSource* old_manual = 
      Context::sourceManager()->get<ManualSource>("manual");

  this->addIntent(NodeConfigIntentRef(new CopyManualSourceIntent()));
  this->load();

  ContextRef new_context = Context::instance();
  ManualSource* new_manual =
      Context::sourceManager()->get<ManualSource>("manual");
  ASSERT_EQ(old_manual, new_manual);
  ASSERT_NE(old_context.get(), new_context.get());
}