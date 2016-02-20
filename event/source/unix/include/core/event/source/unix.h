// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_SOURCE_UNIX_H_
#define CORE_EVENT_SOURCE_UNIX_H_

#include <string>

#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  //! Unix socket source.
  class UnixSource : public sf::core::model::EventSource {
   protected:
    int backlog;
    int socket_fd;
    std::string path;

    void openSocket();

   public:
    UnixSource(std::string socket, std::string id);
    ~UnixSource();

    int getFD();
    sf::core::model::EventRef parse();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_SOURCE_UNIX_H_
