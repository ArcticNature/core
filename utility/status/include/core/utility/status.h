// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_STATUS_H_
#define CORE_UTILITY_STATUS_H_

#include <map>
#include <string>

namespace sf {
namespace core {
namespace utility {

  //! Status colours.
  enum StatusLight {
    UNKOWN = -1,
    GREEN  = 0,
    YELLOW = 1,
    RED    = 2
  };

  //! Details of a status.
  /**
   * \tparam StatusCode enum listing status codes.
   * \tparam START_GREEN  value that starts the GREEN range.
   * \tparam START_YELLOW value that starts the YELLOW range.
   * \tparam START_RED    value that starts the RED range.
   * \tparam END first value out of range.
   */
  template<
    typename StatusCode, StatusCode START_GREEN,
    StatusCode START_YELLOW, StatusCode START_RED, StatusCode END
  >
  class StatusDetail {
   protected:
    StatusCode  _code;
    std::string _message;

   public:
    StatusDetail(StatusCode code, std::string message) {
      this->_code = code;
      this->_message = message;
    }

    StatusCode code() {
      return this->_code;
    }

    std::string message() {
      return this->_message;
    }

    StatusLight colour() {
      if (this->_code >= END) {
        return StatusLight::UNKOWN;
      }
      if (this->_code >= START_RED) {
        return StatusLight::RED;
      }
      if (this->_code >= START_YELLOW) {
        return StatusLight::YELLOW;
      }
      if (this->_code >= START_GREEN) {
        return StatusLight::GREEN;
      }
      return StatusLight::UNKOWN;
    }
  };


  //! Base class for status information.
  template<typename DetailClass>
  class Status {
   public:
    virtual ~Status() {}

    //! Returns the current status colour.
    virtual StatusLight colour() = 0;

    //! Returns the reason for the current colour.
    virtual DetailClass reason() = 0;

  };


  //! Status class that aggregates multiple sub-systems.
  template<typename DetailClass, typename SCode>
  class SubsystemStatus : public Status<DetailClass> {
   public:
    typedef
      typename std::map<std::string, DetailClass>::iterator
      iterator;

   protected:
    std::map<std::string, DetailClass> subsystems;

   public:
    StatusLight colour();
    DetailClass reason();

    //! Sets the state of a subsystem.
    void set(std::string subsys, DetailClass status);

    //! Iterate over subsystems.
    SubsystemStatus::iterator begin();
    SubsystemStatus::iterator end();
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#include "core/utility/status.inc.h"

#endif  // CORE_UTILITY_STATUS_H_
