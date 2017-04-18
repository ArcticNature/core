// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONTEXT_CONTEXT_H_
#define CORE_CONTEXT_CONTEXT_H_

#include <string>

#include "core/compile-time/options.h"
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

    // Context managment function.
    static void Initialise(ContextRef context);
    static ContextRef Instance();

    //! Cleans up all the allocated resources.
    static void Destroy();

   protected:
    sf::core::model::LoggerRef _logger;
    sf::core::model::LoopManagerRef loop_manager;

   public:
    Context();
    ~Context();

    void initialise(sf::core::model::LoggerRef logger);
    void initialise(sf::core::model::LoopManagerRef manager);

    // Instance version of fetch methods.
    // Needed by node configuration to manipulate new context.
    sf::core::model::LoopManagerRef loopManager();
  };

  void CatchPromises();
  void ResetPromiseHandler();

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
