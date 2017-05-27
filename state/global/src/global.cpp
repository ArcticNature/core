// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/state/global.h"

#include "core/cluster/cluster.h"
#include "core/context/context.h"

using sf::core::cluster::Cluster;
using sf::core::context::Context;

using sf::core::state::GlobalState;


GlobalState GlobalState::Capture() {
  GlobalState global;
  global.cluster = Cluster::Instance();
  global.context = Context::Instance();
  return global;
}

void GlobalState::Destroy() {
  Cluster::Destroy();
  Context::Destroy();
}

void GlobalState::restore() {
  Context::Initialise(this->context);
  Cluster::Instance(this->cluster);
}
