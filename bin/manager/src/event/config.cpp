// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager/event/config.h"

#include <exception>
#include <string>

#include "core/bin/manager/api/server.h"
#include "core/cluster/cluster.h"
#include "core/context/context.h"
#include "core/context/static.h"

#include "core/exceptions/base.h"
#include "core/interface/config/node.h"
#include "core/model/event.h"
#include "core/model/logger.h"

#include "core/utility/status.h"

using sf::core::bin::LoadConfiguration;
using sf::core::bin::UnixServer;

using sf::core::cluster::Cluster;
using sf::core::cluster::Node;

using sf::core::context::Context;
using sf::core::context::ProxyLogger;
using sf::core::context::Static;

using sf::core::exception::AbortException;
using sf::core::exception::SfException;

using sf::core::interface::NodeConfigLoader;
using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LogInfo;

using sf::core::utility::Status;
using sf::core::utility::StatusLight;


static ProxyLogger logger("core.bin.manager.event.config");


LoadConfiguration::LoadConfiguration(
    std::string version, EventDrainRef drain, bool abort
) : Event("", drain) {
  this->abort = abort;
  this->version = version;
}

void LoadConfiguration::handle() {
  // Update status.
  Node myself = Cluster::Instance()->myself();
  std::string current = myself->configVersion().effective();
  LogInfo info = {
    {"current", myself->configVersion().effective()},
    {"requested", this->version}
  };
  INFOV(
      logger,
      "Loading configuration '${requested}' (currently at '${current}').",
      info
  );
  myself->status()->set("configuration", Status(
      StatusLight::YELLOW,
      "Loading configuration version '" + this->version + "'"
  ));

  // Load manager configuration.
  NodeConfigLoader conf(this->version);
  conf.load();

  // Update status again.
  std::string requested = myself->configVersion().effective();
  myself->status()->set("configuration", Status(
      StatusLight::GREEN,
      "Applied configuration version '" + requested + "'"
  ));
  info = {{"requested", requested}};
  INFOV(logger, "Configuration '${requested}' loaded.", info);

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
