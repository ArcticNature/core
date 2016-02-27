// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_DAEMON_H_
#define CORE_BIN_DAEMON_H_

#include <string>

#include "core/bin/async-process.h"
#include "core/context/daemon.h"

#include "core/event/source/signal.h"
#include "core/event/source/unix.h"

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

    void createSockets();
    void forkManager();
    void forkSpawner();

    void configureEvents();
    void disableSignals();

    std::string findManager();
    std::string findSpawner();

   public:
    void initialise();
  };


  //! Signal event source for the Daemon process.
  class DaemonSignalSource : public sf::core::event::SignalSource {
   protected:
    sigset_t getSignalsMask();
    sf::core::model::EventRef handleSignal(int signo);

    sf::core::model::EventRef handleChild();
    sf::core::model::EventRef handleReloadConfig();
    sf::core::model::EventRef handleState();
    sf::core::model::EventRef handleStop();

   public:
    DaemonSignalSource();
  };

  //! Handles a child termination.
  class SigChild : public sf::core::model::Event {
   protected:
    bool checkChild(pid_t pid);

   public:
    explicit SigChild(std::string correlation);
    void handle();
  };

  //! Handles daemon termination.
  class TerminateDaemon : public sf::core::model::Event {
   protected:
    sf::core::context::DaemonRef context;

    //! Kills the subpocesses and stops immediatelly.
    void rudeStop();

    //! Request the termination of the manager.
    void stopManager();

    //! Request the termination of the spawner.
    void stopSpawner();

   public:
    explicit TerminateDaemon(std::string correlation);
    void handle();
  };


  //! Unix event source for the Spawner.
  class DaemonSpawnerSource : public sf::core::event::UnixSource {
   protected:
    sf::core::model::EventDrainRef clientDrain(int fd, std::string id);
    sf::core::model::EventSourceRef clientSource(int fd, std::string id);

   public:
    explicit DaemonSpawnerSource(std::string path);
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_DAEMON_H_
