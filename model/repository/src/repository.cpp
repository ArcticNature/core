// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/repository.h"

#include <sstream>
#include <string>
#include <vector>

#include "core/exceptions/base.h"
#include "core/utility/string.h"

using sf::core::exception::ServiceNotFound;
using sf::core::model::Repository;
using sf::core::model::RepositoryVersion;
using sf::core::model::RepositoryVersionId;
using sf::core::model::RepositoryVersionRef;

using sf::core::utility::string::join;
using sf::core::utility::string::split;


Repository::~Repository() {
  // Noop.
}

RepositoryVersionRef Repository::lookup(std::string version) {
  if (version == "<latest>" || version == "") {
    return this->latest();
  }
  this->verifyVersion(version);
  return this->version(version);
}


RepositoryVersionId::RepositoryVersionId(
    std::string effective, std::string symbolic
) {
  this->_effective = effective;
  this->_symbolic  = symbolic;
}

std::string RepositoryVersionId::effective() const {
  return this->_effective;
}

std::string RepositoryVersionId::symbolic() const {
  return this->_symbolic;
}


RepositoryVersion::~RepositoryVersion() {
  // Noop.
}

std::string RepositoryVersion::findDefinitionFile(
    const std::string service, const std::string container
) {
  std::vector<std::string> parts = split(service, '.');
  for (int idx=parts.size(); idx > 0; idx--) {
    std::stringstream buffer;
    buffer << container << '/';
    buffer << join(parts, '/', idx);
    std::string path = buffer.str();

    // Look for lua files.
    std::string lua_path = path + ".lua";
    if (this->exists(lua_path)) {
      return lua_path;
    }

    // TODO(stefano): Remove support for this extention.
    // Look for legacy service file.
    std::string service_path = path + ".service";
    if (this->exists(service_path)) {
      return service_path;
    }
  }
  throw ServiceNotFound("Unable to find definition for " + service);
}
