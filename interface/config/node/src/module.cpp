// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/interface/config/node.h"
#include "core/interface/config/node/intents/manual-source.h"

#include "core/interface/lifecycle.h"
#include "core/model/logger.h"
#include "core/utility/lua.h"

using sf::core::context::Context;
using sf::core::interface::CopyManualSourceIntent;
using sf::core::interface::NodeConfigIntentLuaProxy;
using sf::core::interface::NodeConfigIntentRef;
using sf::core::interface::NodeConfigLoader;

using sf::core::lifecycle::NodeConfigLifecycleArg;
using sf::core::lifecycle::NodeConfigLifecycleHandler;

using sf::core::model::LogInfo;
using sf::core::utility::Lua;
using sf::core::utility::LuaTable;


const std::vector<std::string> CORE_ATTRIBUTES = {
  "event_manager",
  "logger"
};


class NodeLuaCollect : public NodeConfigLifecycleHandler {
 protected:
  void collectCoreIntents(NodeConfigLoader* loader, Lua* lua) {
    // Collect known `core.*` intents.
    std::vector<std::string>::const_iterator attr;
    LuaTable core = lua->globals()->toTable("core");
    NodeConfigIntentLuaProxy lua_intents(*lua);

    for (attr = CORE_ATTRIBUTES.begin();
         attr != CORE_ATTRIBUTES.end(); attr++) {
      core.toStack(*attr);

      // If value is a function call it without arguments.
      if (lua->stack()->type() == LUA_TFUNCTION) {
        LogInfo info = {{"attribute", *attr}};
        DEBUGV(
            Context::logger(),
            "Resolving function in 'core.${attribute}' ...", info
        );
        lua->call(0, 1);
      }

      // Skip if not the correct userdata.
      // TODO(stefano): guard against incorrect userdata type.
      if (lua->stack()->type() != LUA_TUSERDATA) {
        lua->stack()->remove(-1);
        LogInfo info = {{"attribute", *attr}};
        DEBUGV(
            Context::logger(),
            "Skipping 'core.${attribute}' due to incorrect type.", info
        );
        continue;
      }
      loader->addIntent(lua_intents.get());
    }
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
