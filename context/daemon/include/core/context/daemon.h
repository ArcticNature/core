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
    int exit_code;
    sf::core::utility::SubProcessRef manager;
    sf::core::utility::SubProcessRef spawner;

    Daemon();

   public:
    ~Daemon();

    void setManager(sf::core::utility::SubProcessRef manager);
    void setSpawner(sf::core::utility::SubProcessRef spawner);
  };

}  // namespace context
}  // namespace core
}  // namespace sf

#endif  // CORE_CONTEXT_DAEMON_H_
