// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/context.h"
#include "core/interface/config/node.h"
#include "core/interface/config/node/intents/manual-source.h"

#include "core/interface/lifecycle.h"
#include "core/model/logger.h"
#include "core/utility/lua.h"

using sf::core::context::Context;
using sf::core::interface::CopyManualSourceIntent;
using sf::core::interface::NodeConfigLoader;
using sf::core::interface::NodeConfigIntentRef;

using sf::core::lifecycle::NodeConfigLifecycleArg;
using sf::core::lifecycle::NodeConfigLifecycleHandler;
using sf::core::utility::Lua;


class NodeLuaCollect : public NodeConfigLifecycleHandler {
 protected:
  void collectCoreIntents(NodeConfigLoader* loader, Lua* lua) {
    // TODO(stefano): collect `core.*` intents.
  }

  void collectDefaults(NodeConfigLoader* loader) {
    // Move ManualSource and add ScheduledSource if missing.
    loader->addIntent(
        NodeConfigIntentRef(new CopyManualSourceIntent())
    );
  }

 public:
  void handle(std::string event, NodeConfigLifecycleArg* arg) {
    DEBUG(Context::logger(), "Collecting node configuration intents ...");

    // Collect intents and defaults.
    NodeConfigLoader* loader = arg->loader();
    Lua* lua = arg->lua();
    this->collectCoreIntents(loader, lua);
    this->collectDefaults(loader);

    DEBUG(Context::logger(), "Node configuration intents collected!");

  }
};


LifecycleStaticOn("config::node::collect", NodeLuaCollect);
