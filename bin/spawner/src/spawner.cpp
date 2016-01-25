// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/spawner.h"

#include "core/model/logger.h"


using sf::core::bin::Spawner;
using sf::core::model::Logger;


void Spawner::initialise() {
  this->registerDefaultSourceManager();
}
