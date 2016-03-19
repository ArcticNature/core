// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/client/unix.h"

#include <string>

#include "core/context/static.h"
#include "core/model/event.h"


using sf::core::context::Static;

using sf::core::event::FdSource;
using sf::core::event::UnixClient;

using sf::core::model::EventDrainRef;


UnixClient::UnixClient(int fd, std::string id, std::string drain)
  : FdSource(fd, id + "-unix-client", drain) {
  // NOOP.
}
