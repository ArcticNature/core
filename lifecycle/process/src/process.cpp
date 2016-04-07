// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/lifecycle/process.h"

#include "core/interface/lifecycle.h"

using sf::core::interface::Lifecycle;
using sf::core::lifecycle::Process;


void Process::Exit() {
  Lifecycle::trigger("process::exit");
}

void Process::Init() {
  Lifecycle::trigger("process::init");
}
