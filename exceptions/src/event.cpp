// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#include "core/exceptions/event.h"

#include <string>

using sf::core::exception::SfException;
using sf::core::exception::EventDrainNotFound;
using sf::core::exception::EventSourceNotFound;


EventDrainNotFound::EventDrainNotFound(std::string drain) : SfException(
    "Event drain '" + drain + "' not found."
) {}

int EventDrainNotFound::getCode() const {
  return -28;
}


EventSourceNotFound::EventSourceNotFound(std::string source) : SfException(
    "Event source '" + source + "' not found."
) {}

int EventSourceNotFound::getCode() const {
  return -30;
}
