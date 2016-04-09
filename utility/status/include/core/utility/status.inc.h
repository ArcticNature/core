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
    DetailClass* unkown = nullptr;
    DetailClass  reason(SCode::UNKOWN, "no subsystem available");
    typename std::map<std::string, DetailClass>::iterator it;

    // Worse code wins.
    for (it = this->subsystems.begin(); it != this->subsystems.end(); it++) {
      DetailClass subsys = it->second;
      if (subsys.code() == SCode::UNKOWN) {
        unkown = &subsys;
      }
      if (subsys.code() > reason.code()) {
        reason = subsys;
      }
    }

    // But unkown wins over GREEN.
    if (reason.colour() == StatusLight::GREEN && unkown) {
      return *unkown;
    }
    return reason;
  }

  template<typename DetailClass, typename SCode>
  void SubsystemStatus<DetailClass, SCode>::set(
      std::string subsys, DetailClass status
  ) {
    this->subsystems.insert(std::make_pair(subsys, status));
  }

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_STATUS_INC_H_
