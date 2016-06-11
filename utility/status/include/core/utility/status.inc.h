// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_STATUS_INC_H_
#define CORE_UTILITY_STATUS_INC_H_

#include <map>
#include <string>

namespace sf {
namespace core {
namespace utility {

  template<typename DetailClass, typename SCode>
  StatusLight SubsystemStatus<DetailClass, SCode>::colour() {
    bool unkown = false;
    StatusLight colour = StatusLight::UNKOWN;
    typename std::map<std::string, DetailClass>::iterator it;

    // Worse colour wins.
    for (it = this->subsystems.begin(); it != this->subsystems.end(); it++) {
      StatusLight sub_colour = it->second.colour();
      unkown = sub_colour == StatusLight::UNKOWN;
      if (sub_colour > colour) {
        colour = sub_colour;
      }
    }

    // But unkown wins over GREEN.
    if (colour == StatusLight::GREEN && unkown) {
      return StatusLight::UNKOWN;
    }
    return colour;
  }

  template<typename DetailClass, typename SCode>
  DetailClass SubsystemStatus<DetailClass, SCode>::reason() {
    DetailClass reason(SCode::UNKOWN, "no subsystem available");
    DetailClass unkown = reason;
    bool have_unkown = false;
    typename std::map<std::string, DetailClass>::iterator it;

    // Worse code wins.
    for (it = this->subsystems.begin(); it != this->subsystems.end(); it++) {
      DetailClass subsys = it->second;
      if (subsys.code() == SCode::UNKOWN) {
        unkown = subsys;
        have_unkown = true;
      }
      if (subsys.code() > reason.code()) {
        reason = subsys;
      }
    }

    if (reason.colour() == StatusLight::GREEN && have_unkown) {
      return unkown;  // Unkown wins over GREEN.
    }
    if (reason.colour() == StatusLight::UNKOWN && have_unkown) {
      return unkown;  // Avoid the default if possible.
    }
    return reason;
  }

  template<typename DetailClass, typename SCode>
  void SubsystemStatus<DetailClass, SCode>::set(
      std::string subsys, DetailClass status
  ) {
    if (this->subsystems.find(subsys) == this->subsystems.end()) {
      this->subsystems.insert(std::make_pair(subsys, status));
    } else {
      this->subsystems.find(subsys)->second = status;
    }
  }

  template<typename DetailClass, typename SCode>
  typename SubsystemStatus<DetailClass, SCode>::iterator
  SubsystemStatus<DetailClass, SCode>::begin() {
    return this->subsystems.begin();
  }

  template<typename DetailClass, typename SCode>
  typename SubsystemStatus<DetailClass, SCode>::iterator
  SubsystemStatus<DetailClass, SCode>::end() {
    return this->subsystems.end();
  }

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_STATUS_INC_H_
