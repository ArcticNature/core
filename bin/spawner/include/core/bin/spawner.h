// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_SPAWNER_H_
#define CORE_BIN_SPAWNER_H_

#include <string>

#include "core/bin/async-process.h"
#include "core/event/client/unix.h"
#include "core/event/source/unix.h"

#include "core/model/event.h"
#include "core/protocols/daemon_spanwer/ds_message.pb.h"


namespace sf {
namespace core {
namespace bin {

  //! Spawner core implementataion.
  class Spawner : public AsyncPorcess {
   protected:
    void connectDaemon();
    void managerSocket();

   public:
    void initialise();
  };


  //! Spawner to Daemon source.
  class SpawnerToDaemon : public sf::core::event::UnixClient {
   protected:
    sf::core::model::EventRef process(
        sf::core::protocol::daemon_spanwer::Message message
    );

   public:
    static std::string Connect(std::string path);

    SpawnerToDaemon(int fd, std::string id, std::string drain_id);
    sf::core::model::EventRef parse();
  };


  //! Unix event source for the Manager.
  class SpawnerToManagerSource : public sf::core::event::UnixSource {
   protected:
    sf::core::model::EventDrainRef clientDrain(int fd, std::string id);
    sf::core::model::EventSourceRef clientSource(
        int fd, std::string id, std::string drain_id
    );

   public:
    explicit SpawnerToManagerSource(std::string path);
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_SPAWNER_H_
