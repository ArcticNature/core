// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#include "core/exceptions/options.h"

#include <string>


using sf::core::exception::InvalidOption;
using sf::core::exception::SfException;
using sf::core::exception::VariableNotFound;


VariableNotFound::VariableNotFound(std::string msg) : SfException(
    "Variable '" + msg + "' not found."
) { }

int VariableNotFound::getCode() const {
  return -21;
}

MSG_DEFINITION(SfException, InvalidOption, -29);
