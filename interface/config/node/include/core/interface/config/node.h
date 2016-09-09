// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_INTERFACE_CONFIG_NODE_H_
#define CORE_INTERFACE_CONFIG_NODE_H_

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/interface/lifecycle.h"
#include "core/model/repository.h"

#include "core/utility/lua.h"
#include "core/utility/lua/proxy.h"

namespace sf {
namespace core {
namespace interface {

  //! Represents a node configuration step.
  class NodeConfigIntent {
   protected:
    const std::string _id;

   public:
    explicit NodeConfigIntent(std::string id);

    //! Returns the intent id.
    const std::string id() const;

    //! Returns a list of features that must be configured before this.
    /*!
     * To allow intents to be executed in the correct order reguardless of
     * when they are created in the configuration, intents have dependencies
     * and provide features.
     *
     * Features are identifiers of abstract features of the system and not
     * of specific implementation of those features (i.e, `event.manager`
     * rather then `event.manager.epoll`).
     */
    virtual std::vector<std::string> depends() const = 0;

    //! Returns the id of the feature configured by this intent.
    /*!
     * Each intent can configure at most one feature.
     * If no feature configured by the intent is exposed as a dependency
     * to other intents, return the empty string.
     */
    virtual std::string provides() const = 0;

    //! Acts upon the intent by configuring the given context.
    virtual void apply(sf::core::context::ContextRef context) = 0;

    //! Checks that the intent can by applied successfully.
    virtual void verify(sf::core::context::ContextRef context) = 0;
  };
  typedef std::shared_ptr<NodeConfigIntent> NodeConfigIntentRef;


  //! Node configuration loader.
  /*!
   * Host of the Lua environment that runs the configuration process.
   * This base class does not trigger lifecycle events to avoid
   * cyclic dependencies with the `core.lifecycle.config` component.
   *
   * The class also drives the configuration loading process:
   *
   *   01. Initialise the Lua environment (lifecycle event).
   *   02. Create a new empty context.
   *   03. Fetch paths to config files.
   *   04. For each collected file run it in the environment.
   *   05. Extract all generated intents (lifecycle event).
   *   06. Append some special Intents:
   *     * Manaual source (carry over or enqueued events would be lost).
   *     * Lifecycle event.
   *   07. Sort Intents based on depents/provides.
   *   08. Verify all Intents.
   *   09. Apply Intents to new context.
   *   10. Swap contexts out.
   *   11. Update node's configuration version.
   */
  class NodeConfigLoader {
   protected:
    bool loaded;
    std::string effective;
    std::string symbolic;

    sf::core::context::ContextRef new_context;
    sf::core::model::RepositoryVersionRef repo_ver;
    sf::core::utility::Lua lua;

    std::deque<sf::core::interface::NodeConfigIntentRef> intents;
    std::map<std::string, sf::core::interface::NodeConfigIntentRef> providers;

    void apply();
    void initialise();
    void run();
    void sort();
    void updateSystem();
    void verify();

    //! Collects intents configured in the environment.
    virtual void collectIntents();

    //! Initialise the Lua environment.
    virtual void initLua();
    void initLuaFunctions();
    void initLuaTables();
    // TODO(stefano): once all the example options are implemented
    // remove these patches and the method.
    void initLuaPatches();

    //! Returns a list of paths in the repository to load.
    virtual std::vector<std::string> paths() const;

   public:
    explicit NodeConfigLoader(std::string symbolic);

    //! Apps and intent to the list of configuration steps.
    void addIntent(NodeConfigIntentRef intent);

    //! Run the configuration loading process.
    void load();

    //! Throws a MissingConfiguration exception if the provider is missing.
    void requireProvider(std::string id);
  };


  //! Lua type proxy for node configuration intents.
  class NodeConfigIntentLuaProxy : public sf::core::utility::LuaTypeProxy {
   protected:
    static const luaL_Reg lib[];
    static void delete_instance(void* instance);

   protected:
    std::string typeId() const;
    sf::core::utility::lua_proxy_delete deleter() const;
    const luaL_Reg* library() const;

   public:
    NodeConfigIntentLuaProxy();
    explicit NodeConfigIntentLuaProxy(sf::core::utility::Lua lua);

    NodeConfigIntentRef get(int idx = -1, bool pop = true);
    void wrap(sf::core::utility::Lua lua, NodeConfigIntent* intent);
    void wrap(sf::core::utility::Lua lua, NodeConfigIntentRef intent);
  };

}  // namespace interface
}  // namespace core
}  // namespace sf


namespace sf {
namespace core {
namespace lifecycle {

  //! Argument to config::node::* lifecycle events.
  class NodeConfigLifecycleArg : public sf::core::interface::BaseLifecycleArg {
   protected:
    sf::core::utility::Lua* _lua;
    sf::core::interface::NodeConfigLoader* _loader;

   public:
    NodeConfigLifecycleArg(
        sf::core::interface::NodeConfigLoader* loader,
        sf::core::utility::Lua* lua
    );

    //! The node configuration loader instance driving the process.
    sf::core::interface::NodeConfigLoader* loader() const;

    //! The Lua environment used to load and run the config.
    sf::core::utility::Lua* lua() const;
  };

}  // namespace lifecycle
}  // namespace core
}  // namespace sf

#endif  // CORE_INTERFACE_CONFIG_NODE_H_
