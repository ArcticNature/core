// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#include "core/exceptions/event.h"

#include <string>

using sf::core::exception::SfException;
using sf::core::exception::DuplicateEventDrain;
using sf::core::exception::DuplicateEventSource;
using sf::core::exception::EventDrainNotFound;
using sf::core::exception::EventSourceNotFound;
using sf::core::exception::IncorrectSourceType;


DuplicateEventDrain::DuplicateEventDrain(std::string drain) : SfException(
    "Event drain '" + drain + "' already exists."
) {}

int DuplicateEventDrain::getCode() const {
  return -33;
}


DuplicateEventSource::DuplicateEventSource(std::string source) : SfException(
    "Event source '" + source + "' already exists."
) {}

int DuplicateEventSource::getCode() const {
  return -34;
}


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
  return -31;
}


IncorrectSourceType::IncorrectSourceType(std::string source) : SfException(
    "Event source '" + source + "' has incompatible type."
) {}

int IncorrectSourceType::getCode() const {
  return -32;
}
