// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/subprocess.h"

#include <string>

using sf::core::utility::Forker;
using sf::core::utility::SubProcess;


int SubProcess::child() {
  this->closeNonStdFileDescriptors();
  // Clear signals.
  // Redirect std streams.
  // Build command.
  // Exec* to new process.
  return 0;
}

int SubProcess::parent() {
  // In async mode, return.
  if (!this->_wait) {
    return 0;
  }

  // In sync mode, wayt for the child to terminate.
  return 0;
}


SubProcess::SubProcess(std::string binary) : Forker() {
  this->_wait  = false;
  this->binary = binary;
}

void SubProcess::appendArgument(std::string argument) {
  this->arguments.push_back(argument);
}

void SubProcess::run() {
  this->_wait = false;
  this->fork();
}

int SubProcess::wait() {
  this->_wait = true;
  return this->fork();
}
