// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_MANAGER_H_
#define CORE_BIN_MANAGER_H_

#include "core/bin/async-process.h"
#include "core/event/source/unix.h"
#include "core/model/event.h"
#include "core/protocols/daemon_manager/dm_message.pb.h"

namespace sf {
namespace core {
namespace bin {

  //! Manager core implementataion.
  class Manager : public AsyncPorcess {
   protected:
    void connectDaemon();

   public:
    void initialise();
  };


  //! Manager to Daemon source.
  class ManagerToDaemon : public sf::core::event::UnixClient {
   protected:
    sf::core::model::EventRef process(
        sf::core::protocol::daemon_manager::Message message
    );

   public:
    explicit ManagerToDaemon(std::string path);
    sf::core::model::EventRef parse();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_MANAGER_H_
