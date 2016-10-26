// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_SOURCE_SIGNAL_H_
#define CORE_EVENT_SOURCE_SIGNAL_H_

#include <sys/signalfd.h>
#include <unistd.h>

#include <string>

#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  //! Event source triggered by process signals.
  class SignalSource : public sf::core::model::EventSource {
   protected:
    int signal_fd;
    sf::core::model::EventRef parse();

    //! Initialises the signalfd to return to the event manager.
    void createSignalFd();

    //! Returns the set of signals to mask and wait on.
    virtual sigset_t getSignalsMask();

    //! Returns the appropriate event to handle with the signal.
    virtual sf::core::model::EventRef handleSignal(int signo);

    //! Returns the event responable for configuration reload.
    virtual sf::core::model::EventRef handleReloadConfig() = 0;

    //! Returns the event responsible for the status display.
    virtual sf::core::model::EventRef handleState() = 0;

    //! Returns the event responable terminating of the system.
    virtual sf::core::model::EventRef handleStop() = 0;

   public:
    explicit SignalSource(std::string id);
    ~SignalSource();
    int fd();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_SOURCE_SIGNAL_H_
