// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/cluster/cluster.h"
#include "core/context/context.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/exceptions/event.h"

#include "core/interface/lifecycle.h"
#include "core/interface/metadata/store.h"
#include "core/model/event.h"
#include "core/model/logger.h"


using sf::core::cluster::Cluster;
using sf::core::cluster::ClusterRaw;

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::context::Static;

using sf::core::exception::ContextUninitialised;
using sf::core::exception::DuplicateEventDrain;

using sf::core::interface::Lifecycle;
using sf::core::interface::LifecycleHandlerRef;
using sf::core::interface::MetaDataStoreRef;
using sf::core::interface::NoMetadataStore;

using sf::core::lifecycle::EVENT_DRAIN_ENQUEUE;
using sf::core::lifecycle::DrainEnqueueArg;
using sf::core::lifecycle::FlushEventDrain;

using sf::core::model::Event;
using sf::core::model::EventDrain;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManager;
using sf::core::model::LoopManagerRef;

using sf::core::model::Logger;
using sf::core::model::LoggerRef;
using sf::core::model::LogInfo;
using sf::core::model::LogLevel;


class ContextTest : public ::testing::Test {
 protected:
  ContextRef context;

 public:
  ContextTest() {
    this->context = ContextRef(new Context());
    Context::Initialise(this->context);
  }
  ~ContextTest() {
    Context::Destroy();
    Logger::destroyFallback();
  }
};


class NoopLogger : public Logger {
 public:
  NoopLogger() : Logger("") {}

  void log(
      const LogLevel level, const std::string message,
      LogInfo variables
  ) {
    // Noop
  }
};


class NoopLoopManager : public LoopManager {
  public:
   std::map<std::string, EventDrainRef> drains_map;

   void add(EventDrainRef drain) {
     if (this->drains_map.find(drain->id()) != this->drains_map.end()) {
       throw DuplicateEventDrain(drain->id());
     }
     this->drains_map[drain->id()] = drain;
   }

   void add(EventSourceRef source) {}
   void removeDrain(std::string id) {}
   void removeSource(std::string id) {}
   EventRef wait(int timeout = -1) {}
};


TEST_F(ContextTest, Defaults) {
  ASSERT_EQ(Logger::fallback(), Context::Logger());
  ASSERT_THROW(Context::Cluster(), ContextUninitialised);
  ASSERT_THROW(Context::LoopManager(), ContextUninitialised);
  ASSERT_THROW(Context::Metadata(), ContextUninitialised);
}

TEST_F(ContextTest, ReplaceActiveContext) {
  ContextRef context(new Context());
  ASSERT_NE(context, Context::Instance());

  Context::Initialise(context);
  ASSERT_EQ(context, Context::Instance());
}

TEST_F(ContextTest, SetCluster) {
  MetaDataStoreRef metadata = std::make_shared<NoMetadataStore>();
  Cluster cluster = std::make_shared<ClusterRaw>(metadata);
  this->context->initialise(cluster);
  ASSERT_EQ(cluster, Context::Cluster());
}

TEST_F(ContextTest, SetLogger) {
  LoggerRef logger(new NoopLogger());
  this->context->initialise(logger);
  ASSERT_EQ(logger, Context::Logger());
}

TEST_F(ContextTest, SetMetadata) {
  MetaDataStoreRef metadata = std::make_shared<NoMetadataStore>();
  this->context->initialise(metadata);
  ASSERT_EQ(metadata, Context::Metadata());
}

TEST_F(ContextTest, SetSourceManager) {
  LoopManagerRef manager(new NoopLoopManager());
  this->context->initialise(manager);
  ASSERT_EQ(manager, Context::LoopManager());
}


class FlushDrainTest : public ::testing::Test {
 protected:
  ContextRef context;
  NoopLoopManager* manager;

 public:
  FlushDrainTest() {
    this->manager = new NoopLoopManager();
    this->context = Context::Instance();
    this->context->initialise(LoopManagerRef(this->manager));

    Lifecycle::reset();
    Lifecycle::on(
        EVENT_DRAIN_ENQUEUE,
        LifecycleHandlerRef(new FlushEventDrain())
    );
  }

  ~FlushDrainTest() {
    Context::Destroy();
    Logger::destroyFallback();
    Static::destroy();
  }
};


class StubDrain : public EventDrain {
 public:
  StubDrain() : EventDrain("stub-drain") {
    // Noop
  }

  int fd() {
    return 0;
  }

  bool flush() {
    return false;
  }
};


TEST_F(FlushDrainTest, AddDrain) {
  EventDrainRef drain(new StubDrain());
  DrainEnqueueArg arg(drain->id());
  Static::drains()->add(drain->id(), drain);
  Lifecycle::trigger(EVENT_DRAIN_ENQUEUE, &arg);
  ASSERT_TRUE(arg.added());
  ASSERT_EQ(1, this->manager->drains_map.size());
}

TEST_F(FlushDrainTest, DrainNotFound) {
  DrainEnqueueArg arg("not-a-drain");
  Lifecycle::trigger(EVENT_DRAIN_ENQUEUE, &arg);
  ASSERT_FALSE(arg.added());
  ASSERT_EQ(0, this->manager->drains_map.size());
}

TEST_F(FlushDrainTest, SkipExistingDrain) {
  EventDrainRef drain(new StubDrain());
  DrainEnqueueArg arg(drain->id());
  Static::drains()->add(drain->id(), drain);
  Context::LoopManager()->add(drain);
  Lifecycle::trigger(EVENT_DRAIN_ENQUEUE, &arg);
  ASSERT_FALSE(arg.added());
  ASSERT_EQ(1, this->manager->drains_map.size());
}
