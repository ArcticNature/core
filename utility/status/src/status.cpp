// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/status.h"

#include <string>


using sf::core::utility::Status;
using sf::core::utility::StatusLight;
using sf::core::utility::SubsystemStatus;


Status::Status() {
  this->colour_ = StatusLight::UNKOWN;
  this->message_ = "<not-initialised>";
}

Status::Status(StatusLight colour, std::string message) {
  this->colour_ = colour;
  this->message_ = message;
}

StatusLight Status::colour() const {
  return this->colour_;
}

std::string Status::message() const {
  return this->message_;
}


SubsystemStatus::SubsystemStatus() :
    Status(StatusLight::UNKOWN, "<dynamically computed>") {
  // Noop.
}

StatusLight SubsystemStatus::colour() const {
  return this->overall().colour();
}

std::string SubsystemStatus::message() const {
  return this->overall().message();
}

Status SubsystemStatus::overall() const {
  bool found = false;
  Status worse = Status(StatusLight::UNKOWN, "No subsystem available");

  for (auto pair : this->subsystems_) {
    auto subsys = pair.second;

    // Track the first item.
    if (!found) {
      found = true;
      worse = subsys;

    // UNKOWN wins over GREEN.
    } else if (
        worse.colour() == StatusLight::GREEN &&
        subsys.colour() == StatusLight::UNKOWN
    ) {
      worse = subsys;

    // Non-green && worse status then before.
    } else if (
        subsys.colour() != StatusLight::GREEN &&
        worse.colour() < subsys.colour()
    ) {
      worse = subsys;
    }
  }

  if (worse.colour() == StatusLight::GREEN) {
    return Status(StatusLight::GREEN, "All subsystems are healthy");
  }
  return worse;
}

void SubsystemStatus::set(std::string subsys, Status status) {
  this->subsystems_[subsys] = status;
}

SubsystemStatus::iterator SubsystemStatus::begin() {
  return this->subsystems_.begin();
}

SubsystemStatus::iterator SubsystemStatus::end() {
  return this->subsystems_.end();
}
