// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_REGISTRY_REPOSITORIES_H_
#define CORE_REGISTRY_REPOSITORIES_H_

#include "core/model/cli-parser.h"
#include "core/model/repository.h"
#include "core/registry/base.h"

namespace sf {
namespace core {
namespace registry {

  //! Signature of Repository factories.
  typedef sf::core::model::RepositoryRef (*repo_factory)();

  //! Registry for known repositories.
  class ReposRegistry : public Registry<repo_factory> {
   public:
    static void initStaticContext();
  };

}  // namespace registry
}  // namespace core
}  // namespace sf

#endif  // CORE_REGISTRY_REPOSITORIES_H_
