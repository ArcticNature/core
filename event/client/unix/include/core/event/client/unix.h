// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_CLIENT_UNIX_H_
#define CORE_EVENT_CLIENT_UNIX_H_

#include <string>

#include "core/event/source/fd.h"
#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  //! Unix socket client connector.
  class UnixClient : public FdSource {
   protected:
    UnixClient(int fd, std::string id, std::string drain_id);

   public:
    //! Creates a subclass of the UnixClient and the associated drain.
    /*!
     * Enstablishes a connection through a Unix socket and
     * creates an EventDrain and an EventSource to exchange
     * messages over the channel.
     *
     * \tparam SourceClass A subclass of UnixClient that acts as the source.
     * \tparam DrainClass  A subclass of EventDrain that acts as the drain.
     *
     * \param socket The path to the unix socket.
     * \param id     The id of the source/drain pair.
     *
     * \returns The id of the resulting drain.
     */
    template<class SourceClass, class DrainClass>
    static std::string Connect(std::string socket, std::string id);
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#include "core/event/client/unix.inc.h"

#endif  // CORE_EVENT_CLIENT_UNIX_H_
