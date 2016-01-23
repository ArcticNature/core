// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_DAEMON_H_
#define CORE_BIN_DAEMON_H_

#include <string>

#include "core/model/event.h"
#include "core/utility/daemoniser.h"

namespace sf {
namespace core {
namespace bin {

  //! Instances of this class initialise and run the daemon.
  class Daemon : public sf::core::utility::Daemoniser {
   protected:
    sf::core::model::EventSourceManagerRef sources;

    void cleanEnvironment();
    void daemonise();
    void dropUser();

    void createSocket(std::string path);
    void createSockets();
    void forkManager();
    void forkSpawner();

    void configureEvents();
    void disableSignals();

    void loop();

   public:
    void initialise();
    void run();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_DAEMON_H_
