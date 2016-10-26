// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager/api/drain.h"

#include <string>

#include "core/event/drain/fd.h"
#include "core/model/event.h"

#include "core/protocols/public/p_message.pb.h"
#include "core/utility/protobuf.h"

using sf::core::bin::ApiFdDrain;
using sf::core::event::FdDrain;
using sf::core::model::EventRef;

using sf::core::protocol::public_api::Message;
using sf::core::utility::MessageIO;


ApiFdDrain::ApiFdDrain(int fd, std::string id) : FdDrain(fd, id) {
  // NOOP.
}
