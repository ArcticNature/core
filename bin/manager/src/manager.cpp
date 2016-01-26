// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/manager.h"

using sf::core::bin::Manager;


void Manager::initialise() {
  this->registerDefaultSourceManager();
}
