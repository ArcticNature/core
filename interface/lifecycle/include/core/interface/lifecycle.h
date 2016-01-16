// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_INTERFACE_LIFECYCLE_H_
#define CORE_INTERFACE_LIFECYCLE_H_

#include <list>
#include <map>
#include <memory>
#include <string>

#include "core/exceptions/base.h"
#include "core/compile-time/options.h"


namespace sf {
namespace core {
namespace interface {

  //! Base class for event arguments.
  /*!
   * Events always receive a pointer to a lifecycle argument.
   *
   * If event triggers need data to be passed the event handlers they
   * can pass a subclass of the BaseLifecycleArg for the handler to
   * analyse and downcast as appropriate.
   *
   * If no argument is needed for the subclass a nullptr can be passed.
   */
  class BaseLifecycleArg {
   public:
    virtual ~BaseLifecycleArg();
  };

  //! Definition of a lifecycle event handler.
  class BaseLifecycleHandler {
   public:
    virtual ~BaseLifecycleHandler();

    //! Handles an event
    /**!
     * @param event    The name of the event to handle.
     * @param argument An optional argument given by the event trigger.
     */
    virtual void handle(std::string event, BaseLifecycleArg* argument) = 0;
  };
  typedef std::shared_ptr<BaseLifecycleHandler> LifecycleHandlerRef;

  //! Archive and orchestrator of internal events.
  /*!
   * Core logic of the Lifecycle system.
   * Instances of the class manage the map from event to handlers and
   * dispatch the events when they are triggered.
   */
  class LifecycleInstance {
   protected:
    std::map<std::string, std::list<LifecycleHandlerRef>> handlers;

   public:
    //! Removes an event handler from the register.
    void off(std::string event, LifecycleHandlerRef handler);

    //! Registers an event handler for the given event.
    void on(std::string event, LifecycleHandlerRef handler);

    //! Triggers an event and passes the given argument to the handlers.
    void trigger(std::string event, BaseLifecycleArg* arg = nullptr);

#if DEBUG_BUILD
    //! Remove all events and handlers.
    void reset();
#endif  // DEBUG_BUILD
  };

  //! Singleton proxy for a global LifecycleInstance.
  class Lifecycle {
   protected:
    static LifecycleInstance* instance;
    static LifecycleInstance* getInstance();

   public:
    static void off(std::string event, LifecycleHandlerRef handler);
    static void on(std::string event, LifecycleHandlerRef handler);
    static void trigger(std::string event, BaseLifecycleArg* arg = nullptr);

#if DEBUG_BUILD
    static void reset();
#endif  // DEBUG_BUILD
  };

  //! Lifecycle handler with argument type checking.
  /*!
   * When called, this handler will attempt to downcast the BaseLifecycleArg
   * to the tepmate type and call an overloaded version of the handle
   * function that can use the correct type of argument.
   *
   * If no argument is given, the handler is called with nullptr.
   */
  template<class Argument>
  class LifecycleHandler : public BaseLifecycleHandler {
   public:
    virtual void handle(std::string event, Argument* argument) = 0;
    virtual void handle(std::string event, BaseLifecycleArg* argument) {
      Argument* arg = dynamic_cast<Argument*>(argument);
      if (argument != nullptr && arg == nullptr) {
        throw sf::core::exception::TypeError(
            "Invalid argument type while handling event '" + event + "'."
        );
      }
      this->handle(event, arg);
    }
  };

}  // namespace interface
}  // namespace core
}  // namespace sf

#endif  // CORE_INTERFACE_LIFECYCLE_H_
