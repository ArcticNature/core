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


  //! Status of an entity (composed of colour and message).
  class Status {
   protected:
    StatusLight colour_;
    std::string message_;

   public:
    Status();
    Status(StatusLight colour, std::string message);
    virtual StatusLight colour() const;
    virtual std::string message() const;
  };


  //! Status class that aggregates multiple sub-systems.
  class SubsystemStatus : public Status {
   public:
    using iterator = std::map<std::string, Status>::iterator;

   protected:
    std::map<std::string, Status> subsystems_;

   public:
    SubsystemStatus();

    StatusLight colour() const;
    std::string message() const;

    //! Determines the overall status of all subsystems.
    /*!
     * The overall status looks for the most degraded subsystem
     * registered and reports it status.
     *
     * If there are multiple subsystems in the same status
     * one is picked in an undetermined way and returned.
     *
     * If all the subsystems are GREEN, the returned message is
     * "All subsystems are healthy".
     *
     * If all subsystems are GREEN or UNKOWN the UNKOWN state is
     * returned.
     */
    Status overall() const;

    //! Sets the state of a subsystem.
    void set(std::string subsys, Status status);

    //! Iterate over subsystems.
    SubsystemStatus::iterator begin();
    SubsystemStatus::iterator end();
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_STATUS_H_
