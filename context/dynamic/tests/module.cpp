// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>
#include <poolqueue/Promise.hpp>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/event/source/manual.h"
#include "core/model/logger.h"

#include "core/event/testing.h"
#include "core/posix/user.h"


using poolqueue::Promise;

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::context::Static;

using sf::core::context::CatchPromises;
using sf::core::context::ResetPromiseHandler;

using sf::core::event::ManualSource;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::Logger;
using sf::core::model::LoopManagerRef;

using sf::core::event::TestEpollManager;
using sf::core::posix::User;


class UndeliveredPromiseTest : public ::testing::Test {
 protected:
  ContextRef context;
  ManualSource* manual;

 public:
  UndeliveredPromiseTest() {
    this->context = ContextRef(new Context());
    Static::initialise(new User());
    Context::Initialise(this->context);

    this->context->initialise(LoopManagerRef(new TestEpollManager()));
    this->manual = new ManualSource();
    Context::LoopManager()->add(EventSourceRef(this->manual));
  }

  ~UndeliveredPromiseTest() {
    this->context = ContextRef();
    ResetPromiseHandler();
    Context::Destroy();
    Static::destroy();
    Logger::destroyFallback();
  }
};


TEST_F(UndeliveredPromiseTest, Set) {
  CatchPromises();
  {
    Promise p;
    p.settle(std::make_exception_ptr(std::runtime_error("ABC")));
  }
  EventRef event = Context::LoopManager()->wait(1);
  ASSERT_NE(nullptr, event);
}
