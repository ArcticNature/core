// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager/event/config.h"

#include <exception>
#include <string>

#include "core/bin/manager/api/server.h"
#include "core/cluster/node.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/exceptions/base.h"
#include "core/interface/config/node.h"
#include "core/model/event.h"
#include "core/model/logger.h"

using sf::core::bin::LoadConfiguration;
using sf::core::bin::UnixServer;

using sf::core::cluster::Node;
using sf::core::cluster::NodeStatusCode;
using sf::core::cluster::NodeStatusDetail;

using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::exception::AbortException;
using sf::core::exception::SfException;

using sf::core::interface::NodeConfigLoader;
using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;


LoadConfiguration::LoadConfiguration(
    std::string version, EventDrainRef drain, bool abort
) : Event("", drain) {
  this->abort = abort;
  this->version = version;
}

void LoadConfiguration::handle() {
  // Update status.
  std::string current = Node::me()->configVersion().effective();
  LogInfo info = {
    {"current",  Node::me()->configVersion().effective()},
    {"requested", this->version}
  };
  INFOV(
      Context::Logger(),
      "Loading configuration '${requested}' (currently at '${current}').",
      info
  );
  Node::me()->status()->set("configuration", NodeStatusDetail(
      NodeStatusCode::CONFIG_LOAD,
      "Loading configuration version '" + this->version + "'"
  ));

  // Load manager configuration.
  NodeConfigLoader conf(this->version);
  conf.load();

  // Update status again.
  info = {{"requested", Node::me()->configVersion().effective()}};
  INFOV(
      Context::Logger(), "Configuration '${requested}' loaded).",
      info
  );
  Node::me()->status()->set("configuration", NodeStatusDetail(
      NodeStatusCode::CONFIG_OK,
      "Applied configuration version '" + this->version + "'"
  ));

  // TODO(stefano): remove this when config works.
  EventSourceRef server(new UnixServer("/tmp/snow-fox.socket"));
  Context::LoopManager()->add(server);
}

void LoadConfiguration::rescue(std::exception_ptr ex) {
  if (this->abort) {
    try {
      std::rethrow_exception(ex);
    } catch (SfException& nested) {
      throw AbortException(nested);
    }
  }
  std::rethrow_exception(ex);
}
