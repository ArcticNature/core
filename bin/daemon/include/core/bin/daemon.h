// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_DAEMON_H_
#define CORE_BIN_DAEMON_H_

#include <string>
#include "core/utility/daemoniser.h"

namespace sf {
namespace core {
namespace bin {

  //! Instances of this class initialise and run the daemon.
  class Daemon : public sf::core::utility::Daemoniser {
   protected:
    void daemonise();
    void dropUser();

    void createSocket(std::string path);
    void forkSpawner();
    void forkManager();

    void disableSignals();
    void configureEvents();

   public:
    void initialise();
    void run();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_DAEMON_H_
