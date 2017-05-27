// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_STATE_GLOBAL_H_
#define CORE_STATE_GLOBAL_H_

#include "core/cluster/cluster.h"
#include "core/context/context.h"


namespace sf {
namespace core {
namespace state {

  //! Helper class to store all state instances.
  /*!
   * Usefull during operations such as reconfiguration to ensure
   * that the global state is kept consistent at all times.
   *
   * It accesses and sets singleton instances of other components
   * rather then storing them directly.
   * This way components can depend only on the instances that they
   * need and not all state intances (this avoids lots of cyclic
   * dependencies).
   */
  class GlobalState {
   public:
    //! Create a GlobalState instance referencing the current state.
    static GlobalState Capture();

    //! Destrys all global state singleton in the correct order.
    static void Destroy();

   public:
    //! Store cluster state.
    sf::core::cluster::Cluster cluster;

    //! Store context state.
    sf::core::context::ContextRef context;

    //! Sets the global singleton instances to those in this state.
    void restore();
  };

}  // namespace state
}  // namespace core
}  // namespace sf

#endif  // CORE_STATE_GLOBAL_H_
