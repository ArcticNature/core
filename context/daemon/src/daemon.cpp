// Copyright 2016 Stefano Pogliani
#include "core/context/daemon.h"

#include "core/exceptions/base.h"


using sf::core::exception::DuplicateInjection;
using sf::core::context::Daemon;
using sf::core::context::DaemonRef;
using sf::core::utility::SubProcessRef;


DaemonRef Daemon::_instance;


Daemon::Daemon() {
  this->exit_code = 0;
}

Daemon::~Daemon() {
  if (this->manager) {
    this->manager->join();
  }
  if (this->spawner) {
    this->spawner->join();
  }
}

DaemonRef Daemon::instance() {
  if (!Daemon::_instance) {
    Daemon::_instance = DaemonRef(new Daemon());
  }
  return Daemon::_instance;
}

void Daemon::destroy() {
  Daemon::_instance = DaemonRef();
}

void Daemon::setManager(SubProcessRef manager) {
  if (this->manager) {
    throw DuplicateInjection("Cannot register manager twice");
  }
  this->manager = manager;
}

void Daemon::setSpawner(SubProcessRef spawner) {
  if (this->spawner) {
    throw DuplicateInjection("Cannot register spawner twice");
  }
  this->spawner = spawner;
}
