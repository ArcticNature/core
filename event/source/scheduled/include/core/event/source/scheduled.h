// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_SOURCE_SCHEDULED_H_
#define CORE_EVENT_SOURCE_SCHEDULED_H_

#include <memory>
#include <string>
#include <vector>

#include "core/model/event.h"
#include "core/utility/scored-list.h"

namespace sf {
namespace core {
namespace event {

  //! Type for callbacks registered with the ScheduledSource.
  /*!
   * When a service needs to delay execution for a multiple of the
   * tick unit it registers a callback with the ScheduledSource.
   * The callback is executed after the requested amount of ticks
   * are elapsed.
   * 
   * The callback may return an EventRef if it so whishes.
   * If not it must return an empty EventRef.
   */
  typedef sf::core::model::EventRef (*scheduled_callback)(
      std::shared_ptr<void> closure
  );


  //! Structure to hold the callback for a scheduled event.
  struct ScheduledClosure {
    //! Function to call when the scheduler triggers the event.
    scheduled_callback callback;

    //! Pointer passed as an argument to the callback.
    std::shared_ptr<void> closure;

    //! Re-schedule this callback during configuration updates.
    /*!
     * If this is false (default) the callback is discarded
     * in the event of a manager's reconfiguration.
     *
     * Only one off tasks that need to be executed even after
     * node reconfigurations should set this to true.
     */
    bool keep_on_reconfigure = false;
  };
  typedef sf::core::utility::ScoredList<ScheduledClosure>
    ScheduledClosureList;


  //! Manages time delayed events.
  class ScheduledSource : public sf::core::model::EventSource {
   protected:
    int tick;
    int timer_fd;
    ScheduledClosureList closures;

   public:
    ScheduledSource(int tick);
    ~ScheduledSource();

    //! Returns a list of scored callbacks.
    /*!
     * This can be used to easily re-register callbacks with
     * `keep_on_reconfigure == true` on a new SchedulerSource.
     */
    std::vector<ScheduledClosureList::ScoredValue> keepCallbacks();

    void registerCallback(unsigned int score, ScheduledClosure closure);
    void registerCallback(unsigned int score, scheduled_callback callback);

    int getFD();
    sf::core::model::EventRef parse();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_SOURCE_SCHEDULED_H_
