// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager/api/source.h"

#include <string>

#include "core/event/source/fd.h"
#include "core/model/event.h"

using sf::core::bin::ApiFdSource;
using sf::core::event::FdSource;

using sf::core::model::EventRef;


ApiFdSource::ApiFdSource(
    int fd, std::string id, std::string drain
) : FdSource(fd, id, drain) {
  // NOOP.
}

EventRef ApiFdSource::parse() {
  if (!this->checkFD()) {
    return EventRef();
  }

  // TODO(stefano): Parse message & create event.
  return EventRef();
}
