// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/spawner.h"

using sf::core::bin::Spawner;


void Spawner::initialise() {
  this->registerDefaultSourceManager();
}