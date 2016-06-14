// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/registry/repositories.h"

#include <string>

#include "core/context/static.h"

using sf::core::context::Static;
using sf::core::model::RepositoryRef;
using sf::core::registry::ReposRegistry;


void ReposRegistry::initStaticContext() {
  std::string type = Static::parser()->getString("repo-type");
  RepositoryRef repo = ReposRegistry::Get(type)();
  Static::repository(repo);
}
