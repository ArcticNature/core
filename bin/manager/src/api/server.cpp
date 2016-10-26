// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <string>

#include "core/bin/manager/api/drain.h"
#include "core/bin/manager/api/server.h"
#include "core/bin/manager/api/source.h"

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/model/logger.h"

using sf::core::bin::ApiFdDrain;
using sf::core::bin::ApiFdSource;
using sf::core::bin::UnixServer;

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::UnixSource;
using sf::core::model::EventDrainRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;


EventDrainRef UnixServer::clientDrain(int fd, std::string id) {
  EventDrainRef drain(new ApiFdDrain(fd, id));
  std::string   drain_id = drain->id();
  LogInfo info = {{"drain-id", drain_id}};
  DEBUGV(
      Context::Logger(),
      "Created drain ${drain-id} for unix connection.", info
  );
  return drain;
}

EventSourceRef UnixServer::clientSource(
    int fd, std::string id, EventDrainRef drain
) {
  EventSourceRef source(new ApiFdSource(fd, id, drain));
  std::string    source_id = source->id();
  LogInfo info = {
    {"drain-id",  drain->id()},
    {"source-id", source_id}
  };
  DEBUGV(
      Context::Logger(),
      "Created source ${source-id} for unix connection.", info
  );
  return source;
}


UnixServer::UnixServer(std::string socket)
  : UnixSource(socket, "public-api") {
  // NOOP.
}
