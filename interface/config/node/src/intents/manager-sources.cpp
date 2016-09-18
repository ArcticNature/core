// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/config/node/intents/manager-sources.h"

#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/context/static.h"

using sf::core::context::Context;
using sf::core::context::ContextRef;
using sf::core::context::Static;
using sf::core::interface::CopyManagerSourcesIntent;


const std::vector<std::string> CopyManagerSourcesIntent::DEPS = {
  "event.manager"
};

CopyManagerSourcesIntent::CopyManagerSourcesIntent(
) : NodeConfigIntent("event.source.daemon.*") {
  this->daemon_source = Static::options()->getString("daemon-source-id");
  this->spawner_source = Static::options()->getString("spawner-source-id");
}

void CopyManagerSourcesIntent::apply(ContextRef context) {
  context->instanceSourceManager()->add(
      Context::sourceManager()->fetch(this->daemon_source)
  );
  context->instanceSourceManager()->add(
      Context::sourceManager()->fetch(this->spawner_source)
  );
}

std::vector<std::string> CopyManagerSourcesIntent::depends() const {
  return CopyManagerSourcesIntent::DEPS;
}

std::string CopyManagerSourcesIntent::provides() const {
  return "event.source.daemon.*";
}

void CopyManagerSourcesIntent::verify(ContextRef context) {
  Context::sourceManager()->fetch(this->daemon_source);
  Context::sourceManager()->fetch(this->spawner_source);
}
