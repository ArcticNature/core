// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/config/detail/service_step.h"

#include <poolqueue/Promise.hpp>
#include <string>

#include "core/config/base.h"
#include "core/utility/lua/table.h"


using poolqueue::Promise;

using sf::core::config::ConfigLoader;
using sf::core::config::ConfigStep_;
using sf::core::config::ServiceDescriptionStep;

using sf::core::utility::LuaTable;


ServiceDescriptionStep::ServiceDescriptionStep(
    ConfigLoader loader, std::string version
) : ConfigStep_(loader), version_(version) {
  // Noop.
}

Promise ServiceDescriptionStep::execute() {
  return Promise().settle().then([this]() {
    return this->builder_.build();
  });
}

Promise ServiceDescriptionStep::verify() {
  return Promise().settle().then([this]() {
    // Find service ID and configs.
    LuaTable service = this->lua().globals()->toTable("__service");
    std::string service_id = service.toString("id");
    LuaTable conf = service.toTable("conf");
    LuaTable connector = conf.toTable("connector");

    // Populate core builder options.
    this->builder_.serviceId(service_id);
    this->builder_.configVersion(this->version_);
    this->builder_.connector(connector.toString("id"));

    // Config options (convert table to json).
    connector.remove("id");
    this->builder_.connectorConfig(connector.toJSON());

    // Load environment mappings.
    if (conf.has("environment")) {
      LuaTable env = conf.toTable("environment");
      env.forEach([&env, this](std::string name) {
        this->builder_.environment(name, env.toString(name));
      });
    }

    // Load ports.
    if (conf.has("ports")) {
      LuaTable ports = conf.toTable("ports");
      ports.forEach([&ports, this](std::string name) {
        int port = ports.toInt(name);
        this->builder_.port(name, port);
      });
    }

    // Execution user and group.
    if (conf.has("group")) {
      this->builder_.group(conf.toString("group"));
    }
    if (conf.has("user")) {
      this->builder_.user(conf.toString("user"));
    }

    // Done.
    return nullptr;
  });
}
