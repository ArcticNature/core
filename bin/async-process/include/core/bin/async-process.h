// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_ASYNC_PROCESS_H_
#define CORE_BIN_ASYNC_PROCESS_H_

#include "core/exceptions/base.h"
#include "core/model/event.h"


namespace sf {
namespace core {
namespace bin {

  //! Helper base class for event based processes.
  class AsyncPorcess {
   protected:
    //! Handles an error raised by the event loop.
    void handleLoopError(
        sf::core::model::EventRef event,
        sf::core::exception::SfException* ex,
        bool drain
    );

    //! Runs the event loop.
    void loop();

    //! Registers the default event source manager.
    /*!
     * The default event source manager is specified in
     * the compile time options and will be regitered in
     * the active Context.
     */
    void registerDefaultSourceManager();

   public:
    //! Runs the event loop for the process.
    void run();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_ASYNC_PROCESS_H_
