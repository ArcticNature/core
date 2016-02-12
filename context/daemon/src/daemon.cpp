// Copyright 2016 Stefano Pogliani
#include "core/context/daemon.h"

#include "core/exceptions/base.h"


using sf::core::exception::DuplicateInjection;
using sf::core::context::Daemon;
using sf::core::context::DaemonRef;
using sf::core::utility::SubProcessRef;


DaemonRef Daemon::_instance;


DaemonRef Daemon::instance() {
  if (!Daemon::_instance) {
    Daemon::_instance = DaemonRef(new Daemon());
  }
  return Daemon::_instance;
}

void Daemon::destroy() {
  Daemon::_instance = DaemonRef();
}


Daemon::Daemon() {
  this->_exiting  = false;
  this->exit_code = 0;
  this->forced_exit = false;
}

Daemon::~Daemon() {
  if (this->manager) {
    this->manager->join();
  }
  if (this->spawner) {
    this->spawner->join();
  }
}

int Daemon::exitCode() {
  return this->exit_code;
}

void Daemon::exiting(int code) {
  this->_exiting = true;
  if (code != 0) {
    this->exit_code = code;
  }
}

void Daemon::shutdown() {
  this->_exiting = true;
}

void Daemon::shutdownForced() {
  this->_exiting = true;
  this->forced_exit = true;
}

bool Daemon::terminating() {
  return this->_exiting;
}

bool Daemon::terminatingForced() {
  return this->forced_exit;
}

void Daemon::clearManager() {
  this->manager = SubProcessRef();
}

void Daemon::clearSpawner() {
  this->spawner = SubProcessRef();
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

pid_t Daemon::managerPid() {
  if (this->manager) {
    return this->manager->getChildPid();
  }
  return -1;
}

pid_t Daemon::spawnerPid() {
  if (this->spawner) {
    return this->spawner->getChildPid();
  }
  return -1;
}
