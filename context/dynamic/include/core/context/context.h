// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONTEXT_CONTEXT_H_
#define CORE_CONTEXT_CONTEXT_H_

#include <string>

#include "core/compile-time/options.h"
#include "core/interface/metadata/store.h"
#include "core/model/event.h"
#include "core/model/logger.h"


namespace sf {
namespace core {
namespace context {

  class Context;
  typedef std::shared_ptr<Context> ContextRef;

  //! Context for the processes to access configurable elements.
  class Context {
   protected:
    static ContextRef _instance;

   public:
    static sf::core::model::LoggerRef Logger();
    static sf::core::model::LoopManagerRef LoopManager();
    static sf::core::interface::MetaDataStoreRef Metadata();

    // Context managment function.
    static void Initialise(ContextRef context);
    static ContextRef Instance();

    //! Cleans up all the allocated resources.
    static void Destroy();

   protected:
    sf::core::model::LoggerRef _logger;
    sf::core::model::LoopManagerRef loop_manager;

    sf::core::interface::MetaDataStoreRef metadata_;

   public:
    Context();
    ~Context();

    //! Initialise the context.
    void initialise(sf::core::model::LoggerRef logger);
    void initialise(sf::core::model::LoopManagerRef manager);
    void initialise(sf::core::interface::MetaDataStoreRef metadata);

    // Fetch configured instances.
    sf::core::model::LoopManagerRef loopManager();
    sf::core::interface::MetaDataStoreRef metadata();
  };


  //! Install and remove global promise handling hook.
  void CatchPromises();
  void ResetPromiseHandler();


  //! Proxy log requests to the context logger and add useful variables.
  /*
   * A decorator Logger that can be used as a static file variable which
   * proxies logging request to the current context while also adding the
   * following information to the LogInfo map:
   *
   *   * `component`: A string passed to the proxy constructor.
   */
  class ProxyLogger : public sf::core::model::Logger {
   protected:
    std::string component_;

   public:
    explicit ProxyLogger(std::string component);

    //! Provided for compatibility with logging macros.
    /*!
     * The logging macros where introduced prior to the
     * idea of the `ProxyLogger` and were designed for
     * `LoggerRef`s.
     *
     * To allow `ProxyLogger` and `LoggerRef`s to
     * both use the same macros, `ProxyLogger` overrides
     * the `->` operator to return a pointer to itself.
     *
     * This feature **should NOT** be use anywere outside
     * the logging macros and will be removed as soon as
     * it is no longer needed.
     */
    ProxyLogger* operator->();

    //! Decorating log call.
    virtual void log(
        const sf::core::model::LogLevel level,
        const std::string message,
        sf::core::model::LogInfo variables
    );
  };
}  // namespace context
}  // namespace core
}  // namespace sf


namespace sf {
namespace core {
namespace lifecycle {

  //! Handles EVENT_DRAIN_ENQUEUE events.
  /*!
   * This event handler fetches the EventDrainRef for the
   * event drain to add to the loop manager and adds it for
   * flushing during the run loop.
   */
  class FlushEventDrain : public DrainEnqueueHandler {
   public:
    void handle(std::string event, DrainEnqueueArg* argument);
  };

}  // namespace lifecycle
}  // namespace core
}  // namespace sf

#endif  // CORE_CONTEXT_CONTEXT_H_
