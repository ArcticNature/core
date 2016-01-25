// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONTEXT_CONTEXT_H_
#define CORE_CONTEXT_CONTEXT_H_

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
    static sf::core::model::LoggerRef logger();
    static sf::core::model::EventSourceManagerRef sourceManager();


    // Context managment function.
    static void initialise(ContextRef context);
    static ContextRef instance();

#if DEBUG_BUILD
    static void reset();
#endif  // DEBUG_BUILD

   protected:
    sf::core::model::LoggerRef _logger;
    sf::core::model::EventSourceManagerRef source_manager;

   public:
    Context();

    void initialise(sf::core::model::LoggerRef logger);
    void initialise(sf::core::model::EventSourceManagerRef source_manager);
  };

}  // namespace context
}  // namespace core
}  // namespace sf

#endif  // CORE_CONTEXT_CONTEXT_H_
