// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/options.h"

#include <string>

#include "core/exceptions/options.h"

using sf::core::exception::VariableNotFound;
using sf::core::model::Options;


bool Options::getBoolean(std::string name) {
  if (this->booleans.find(name) == this->booleans.end()) {
    throw VariableNotFound(name);
  }
  return this->booleans[name];
}

int Options::getInteger(std::string name) {
  if (this->integers.find(name) == this->integers.end()) {
    throw VariableNotFound(name);
  }
  return this->integers[name];
}

std::string Options::getString(std::string name) {
  if (this->strings.find(name) == this->strings.end()) {
    throw VariableNotFound(name);
  }
  return this->strings[name];
}

void Options::setBoolean(std::string name, bool value) {
  this->booleans[name] = value;
}

void Options::setInteger(std::string name, int value) {
  this->integers[name] = value;
}

void Options::setString(std::string name, std::string value) {
  this->strings[name] = value;
}
