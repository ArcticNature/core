// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_DAEMON_H_
#define CORE_BIN_DAEMON_H_

#include <string>

#include "core/bin/async-process.h"
#include "core/event/source/signal.h"
#include "core/model/event.h"
#include "core/utility/daemoniser.h"

namespace sf {
namespace core {
namespace bin {

  //! Instances of this class initialise and run the daemon.
  class Daemon : public sf::core::utility::Daemoniser, public AsyncPorcess {
   protected:
    void cleanEnvironment();
    void daemonise();
    void dropUser();

    void createSocket(std::string path);
    void createSockets();
    void forkManager();
    void forkSpawner();

    void configureEvents();
    void disableSignals();

   public:
    void initialise();
  };


  //! Signal event source for the Daemon process.
  class DaemonSignalSource : public sf::core::event::SignalSource {
   protected:
    sf::core::model::EventRef handleReloadConfig();
    sf::core::model::EventRef handleState();
    sf::core::model::EventRef handleStop();

   public:
    DaemonSignalSource();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_DAEMON_H_
