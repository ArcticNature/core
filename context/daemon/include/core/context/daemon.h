// Copyright 2016 Stefano Pogliani
#ifndef CORE_CONTEXT_DAEMON_H_
#define CORE_CONTEXT_DAEMON_H_

#include <memory>

#include "core/utility/subprocess.h"


namespace sf {
namespace core {
namespace context {

  class Daemon;
  typedef std::shared_ptr<Daemon> DaemonRef;

  //! Daemon specific context.
  class Daemon {
   protected:
    static DaemonRef _instance;

   public:
    static DaemonRef instance();
    static void destroy();

   protected:
    bool _exiting;
    int  exit_code;
    bool forced_exit;

    sf::core::utility::SubProcessRef manager;
    sf::core::utility::SubProcessRef spawner;

    Daemon();

   public:
    ~Daemon();

    void exiting(int code);
    void shutdown();
    void shutdownForced();
    bool terminating();
    bool terminatingForced();

    void clearManager();
    void clearSpawner();

    void setManager(sf::core::utility::SubProcessRef manager);
    void setSpawner(sf::core::utility::SubProcessRef spawner);

    pid_t managerPid();
    pid_t spawnerPid();
  };

}  // namespace context
}  // namespace core
}  // namespace sf

#endif  // CORE_CONTEXT_DAEMON_H_
