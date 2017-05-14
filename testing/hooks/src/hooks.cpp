// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/testing/hooks.h"


using sf::core::testing::HookTest;


void HookTest::clearHooks() {
  for (auto cb : this->cleaners_) {
    cb();
  }
}
