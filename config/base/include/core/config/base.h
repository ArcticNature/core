// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONFIG_BASE_H_
#define CORE_CONFIG_BASE_H_

#include <poolqueue/Promise.hpp>
#include <memory>

#include "core/hook.h"
#include "core/utility/lua.h"


namespace sf {
namespace core {
namespace config {

  // Forward declare shared_ptr type.
  class ConfigLoader_;
  class ConfigStep_;
  typedef std::shared_ptr<ConfigLoader_> ConfigLoader;
  typedef std::shared_ptr<ConfigStep_> ConfigStep;


  //! Abstract LUA-based configuration loader.
  /*!
   * Configuration loading is a (possibly async) process that
   * returns a promise settling to a configured item, which
   * depends on the specific steps.
   *
   * A ConfigLoader class is responsible for configuring one thing.
   *
   * Components that apply configurations (node, service, ...) will
   * create an appropriate subblcass and will only need to call the
   * `load` method.
   *
   * The promise returned by `load` will resolve to a subclass
   * dependent value (i.e: the node loader will return a global
   * state container, the service loader will return a service
   * description).
   *
   * Next we discuss details from the point of veiw of the
   * configuration process and implementors.
   *
   * The main point is that `ConfigStep`s are returned and
   * glued together by a LUA script.
   * The loading process will return one more or less complex
   * configuration step that will be verified and then executed.
   *
   * The detailed list of steps is:
   *
   *   1. The lua environment is initialised:
   *     1. The virtual `initLuaEnv` method is called.
   *        Subclasses can perform any needed extra logic.
   *     2. The static `sf::core::hook::ConfigLoader::InitialiseLua`
   *        hook is called with the lua environment and the loader.
   *        This can be used by other components to extend the environment.
   *     3. The instance specific `initialiseLua` hook is called.
   *        It works the same way as the static hook but it is not
   *        globally available.
   *   2. Run the LUA script (possibly with async calls).
   *   3. Collect the main `ConfigStep` as instructed by the subclass.
   *      It is the subclass responsibility to group and sort more
   *      complex steps into a single one returned to the loader.
   *   4. Call `verify` on the returned step.
   *   5. Call `execute` on the returned step.
   *   6. Settle the loader promise with the result of the `execute` step.
   *
   *  TODO(stefano): only when feture is needed
   *    - provide multistep step to group ordered nested steps.
   *    - steps depend on features.
   *    - loader tracks available features.
   *    - collection add features as available when steps are found.
   *    - loader supports defaults (feat name => lambda<step()>).
   *    - defaults are executed as soon as a step requires a missing feature.
   *    - multistep steps can be extended with hooks:
   *      - introduce collection priorites.
   *      - collect hook may be called once for each priority.
   */
  class ConfigLoader_ : public std::enable_shared_from_this<ConfigLoader_> {
   public:
    //! Instance specific initialiation hook.
    sf::core::hook::Hook<ConfigLoader, sf::core::utility::Lua> initialiseLua;

    //! Performs the loading steps documented above.
    /*!
     * As some steps are asynchronous, the loading process returns
     * a promise that is resolved to the configured element (i.e,
     * a context, a service description, ...).
     */
    poolqueue::Promise load();

   protected:
    //! The LUA environment driving the configuration.
    sf::core::utility::Lua lua_;

    //! Performs the environment initialisation steps.
    /*!
     * 1. Call `initLuaEnv()` for sublcasses.
     * 2. Trigger global config init hook.
     * 3. Trigger instance config init hook.
     *
     * If sublcasses want to call additional hooks they can do so
     * in their `initLuaEnv` implementation.
     */
    void initialise();

    //! Implemented by classes that want to customise the environment.
    virtual void initLuaEnv();

    //! Subclass specific logic to return a configuration step.
    virtual ConfigStep collectStep() = 0;

    //! Return a promise that settles when the LUA code has run.
    virtual poolqueue::Promise runLua() = 0;

    // Allow ConfigStep_ to access internal data structure so it
    // can provide helper functions to subclasses of its.
    friend ConfigStep_;
  };


  //! Abstract class to verify and execute configuration actions.
  class ConfigStep_ : public std::enable_shared_from_this<ConfigStep_> {
   public:
    explicit ConfigStep_(ConfigLoader loader);
    virtual ~ConfigStep_() = default;

    //! Run the required operation to return a configured instance.
    /*!
     * The value the promise settles to depends on the step itself.
     */
    virtual poolqueue::Promise execute() = 0;

    //! Check that the provided options and requirements are met.
    /*!
     * This promise should be rejected with an exception if
     * verification fails.
     */
    virtual poolqueue::Promise verify() = 0;

   protected:
    //! The loader this step was created by.
    ConfigLoader loader_;

    //! Access the lua environment from the loader.
    sf::core::utility::Lua lua();
  };

}  // namespace config
}  // namespace core
}  // namespace sf

#endif  // CORE_CONFIG_BASE_H_
