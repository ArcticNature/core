// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/subprocess.h"

#include <string>

#include "core/context/static.h"

using sf::core::context::Static;
using sf::core::utility::Forker;
using sf::core::utility::SubProcess;


int SubProcess::child() {
  this->closeNonStdFileDescriptors();
  // Clear signals.
  // Redirect std streams.

  // Build command.
  int argc = 2 + this->arguments.size();  // binary + nullptr
  char** arguments = reinterpret_cast<char**>(
      Static::posix()->malloc(argc * sizeof(char*))
  );

  for (int idx = 1; idx < argc - 1; idx++) {
    arguments[idx] = const_cast<char*>(this->arguments[idx - 1].c_str());
  }
  arguments[0] = const_cast<char*>(this->binary.c_str());
  arguments[argc - 1] = nullptr;

  // Now become the other process.
  Static::posix()->execvp(this->binary.c_str(), arguments);
  return 0;
}

int SubProcess::parent() {
  // In async mode, return.
  if (!this->_wait) {
    return 0;
  }

  // In sync mode, wait for the child to terminate.
  return this->join();
}


SubProcess::SubProcess(std::string binary) : Forker() {
  this->_wait  = false;
  this->binary = binary;
}

void SubProcess::appendArgument(std::string argument) {
  this->arguments.push_back(argument);
}

int SubProcess::join() {
  int status;
  Static::posix()->waitpid(this->getChildPid(), &status, 0);
  if (WIFEXITED(status)) {
    return  WEXITSTATUS(status);
  }
  return WTERMSIG(status) << 8;
}

void SubProcess::run() {
  this->_wait = false;
  this->fork();
}

int SubProcess::wait() {
  this->_wait = true;
  return this->fork();
}
