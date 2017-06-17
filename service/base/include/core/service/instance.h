// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_SERVICE_INSTANCE_H_
#define CORE_SERVICE_INSTANCE_H_


#include <string>

#include "core/service/description.h"


namespace sf {
namespace core {
namespace service {

  //! Represents a service instance.
  /*!
   * An instance represents an executable "entity" the exact nature
   * of which depends on the instance details and the connector.
   * Exampls are local processes, docker containers, ...
   *
   * An instance has two id:
   *
   *   * A unique (128 bits) ID for the instance.
   *   * The ID returned by the connector (pid, container id, ...).
   *
   * Additionally essential metadata are stored:
   *
   *   * The service description (what was/should be run/running).
   *   * The state of the instance (running, failed, ...).
   *   * The node the instance is running on.
   */
  class ServiceInstance {
   public:
    //! Service instance state.
    enum State {
      UNALLOCATED = 0,  //!< The intance still needs to be homed.
      ALLOCATED = 1,    //!< The instance has been allocaded.
      PREPPING = 2,     //!< The target node is preparing to run the service.
      STARTING = 3,     //!< The target node is starting the instance.
      RUNNING = 4,      //!< The instance is running.
      STOPPING = 5,     //!< The instance is stopping.
      STOPPED = 6,      //!< The instance terminated successfully.
      FAILED = 7,       //!< The instance failed at some point.
    };

   public:
    explicit ServiceInstance(ServiceDescription description);
    virtual ~ServiceInstance() = default;

   protected:
    //! Description of the service instance.
    ServiceDescription description_;

    //! Globally unique instance ID.
    std::string global_instance_id_;

    //! The ID of the instance id on the node.
    std::string node_instance_id_;

    //! The ID of the node the instance is running on.
    std::string node_name_;

    //! The state the instance is in.
    ServiceInstance::State state_;
  };

}  // namespace service
}  // namespace core
}  // namespace sf

#endif  // CORE_SERVICE_INSTANCE_H_
