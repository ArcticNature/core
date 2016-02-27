// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_SPAWNER_H_
#define CORE_BIN_SPAWNER_H_

#include <string>

#include "core/bin/async-process.h"
#include "core/event/source/unix.h"

namespace sf {
namespace core {
namespace bin {

  //! Spawner core implementataion.
  class Spawner : public AsyncPorcess {
   protected:
    void connectDaemon();

   public:
    void initialise();
  };


  //! Spawner to daemon source.
  class SpawnerToDaemon : public sf::core::event::UnixClient {
   protected:
    sf::core::model::EventDrainRef clientDrain(int fd, std::string id);
    sf::core::model::EventSourceRef clientSource(int fd, std::string id);

   public:
    explicit SpawnerToDaemon(std::string path);
    sf::core::model::EventRef parse();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_SPAWNER_H_
