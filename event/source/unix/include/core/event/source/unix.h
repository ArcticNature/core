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

    //! Returns the event drain for the client.
    /*!
     * Different protocols and components require serialisation
     * and processing of different messages.
     *
     * To account for that, the client drain is created on behalf
     * of the unix socket source by a subclass.
     *
     * \param fd The client connection fd.
     * \param id The id of the client drain.
     */
    virtual sf::core::model::EventDrainRef clientDrain(
        int fd, std::string id
    ) = 0;

    //! Returns the event source for the client.
    /*!
     * Different protocols and components require serialisation
     * and processing of different messages.
     *
     * To account for that, the client source is created on behalf
     * of the unix socket source by a subclass.
     *
     * \param fd The client connection fd.
     * \param id The id of the client source.
     */
    virtual sf::core::model::EventSourceRef clientSource(
        int fd, std::string id
    ) = 0;

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
