// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_EVENT_H_
#define CORE_MODEL_EVENT_H_

#include <map>
#include <memory>
#include <string>

#include "core/exceptions/base.h"
#include "core/exceptions/event.h"


namespace sf {
namespace core {
namespace model {

  class EventDrain;
  typedef std::shared_ptr<EventDrain> EventDrainRef;


  //! Base event definition.
  /**
   * SnowFox is an event based system that reacts to user requests or
   * external events triggered by processes or operating system.
   * This class is the definition of an Event, the core entity that drives
   * SnowFox rective engine.
   *
   * Each event has a correlation id (to link it to other events that
   * cooperatively react to the user or the system) and a drain id (to
   * know where to send the result of an operation when done).
   */
  class Event {
   protected:
    std::string _id;
    std::string correlation_id;
    std::string drain_id;

   public:
    Event(std::string correlation, std::string drain);
    virtual ~Event();

    //! Returns the event correlation id.
    std::string correlation() const;

    //! Returns the id of the drain for this event.
    std::string drain() const;

    //! Initialises the event id, if missing.
    std::string id(std::string id);

    //! Returns the event id.
    std::string id() const;

    //! Handles the event in the most appropriate way.
    virtual void handle() = 0;

    //! Attempt to rescure form an handling error.
    virtual void rescue(sf::core::exception::SfException* ex);
  };
  typedef std::shared_ptr<Event> EventRef;


  //! Handle the output of an event handling.
  /**
   * When events are handled in the system there may be a need to
   * report information back to the caller.
   * When events are async it becomes difficult to do wait for the
   * handling to be completed before sending this information.
   *
   * When an event is created it will have a drain associated to it.
   * The source decides the drain for the event.
   */
  class EventDrain {
   protected:
    const std::string drain_id;

   public:
    explicit EventDrain(std::string id);
    virtual ~EventDrain();

    //! \returns the ID of the event drain.
    std::string id() const;

    //! \returns The writable file descriptor to send data to.
    virtual int getFD() = 0;

    //! Handles exceptions that were thrown while handling an event.
    /**
     * When an exception is thrown by the handle method of an Event,
     * this method is called with that exception.
     * This method should notify the drain about the error.
     * 
     * It is possible to throw exceptions from within this method
     * but these are not caught.
     * Any exception that is thrown from this method will result in
     * the termination of the server.
     *
     * \returns True to indicate that the exception was handled.
     */
    virtual bool handleError(sf::core::exception::SfException* ex);

    //! Sends an ack down the drain.
    virtual void sendAck() = 0;
  };


  //! Manages a set of event drains.
  /**
   * EventDrains are registerd by the system or EventSources into the
   * EventDrainManager so that Events and other components in the
   * system can used them whenever needed.
   *
   * Sources are repsonible for de-registering Drains when they are
   * no longer needed.
   */
  class EventDrainManager {
   protected:
    std::map<std::string, EventDrainRef> drains;

   public:
    //! Adds an event drain to the set.
    void add(EventDrainRef drain);

    //! \returns an EventDrain by reference.
    EventDrainRef get(std::string id) const;

    //! Removes an EventDrain from the set.
    void remove(std::string id);
  };
  typedef std::shared_ptr<EventDrainManager> EventDrainManagerRef;


  //! Source of Events.
  /**
   * An EventSource watches some aspect of the system or a user interface
   * and triggers events when something happens.
   *
   * An EventSource is responsible for registering any EventDrain that
   * the Events may need to respond to users and to assing an EventDrain
   * to each generated Event.
   */
  class EventSource {
   protected:
    const std::string source_id;

   public:
    explicit EventSource(std::string id);
    virtual ~EventSource();

    //! \returns the ID of the event source.
    std::string id() const;

    //! \returns the file descriptor to wait for events on.
    virtual int getFD() = 0;

    //! Parses an event from the source.
    /**
     * Called when the EventSourceManager determines that the source is ready
     * for reading.
     * \returns an Event to handle or nullptr if no event needs handling.
     */
    virtual EventRef parse() = 0;
  };
  typedef std::shared_ptr<EventSource> EventSourceRef;


  //! Manages event sources.
  /**
   * As the system can receive events from multiple sources, an
   * EventSourceManager is responsible for coordinating the reading
   * and handling od these events.
   *
   * The EventSourceManager instance in use is stored in the Context
   * so there is only one **active** EventSourceManager (but there could
   * be multiple EventSourceManager instances).
   */
  class EventSourceManager {
   protected:
    std::map<std::string, EventSourceRef> sources;

   public:
    virtual ~EventSourceManager();

    //! Returns a pointer to the requested event source.
    template<typename EventSrc>
    EventSrc* get(std::string id) {
      if (this->sources.find(id) == this->sources.end()) {
        throw sf::core::exception::EventSourceNotFound(id);
      }
      EventSourceRef ref = this->sources.at(id);
      EventSrc* source = dynamic_cast<EventSrc*>(ref.get());
      if (source == nullptr) {
        throw sf::core::exception::IncorrectSourceType(id);
      }
      return source;
    }

    //! Adds an event source to the manager.
    virtual void addSource(EventSourceRef source) = 0;

    //! Removes an event source from the manager.
    virtual void removeSource(std::string id) = 0;

    //! Waits for one or more file descriptors to be ready.
    /**
     * The method blocks the execution of the current thread until
     * an event is returned or until timeout expires.
     *
     * 
     * \param timeout How long (in ms) to block for before giving up.
     * The default of -1 will block indefinitely.
     */
    virtual EventRef wait(int timeout = -1) = 0;
  };
  typedef std::shared_ptr<EventSourceManager> EventSourceManagerRef;

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_EVENT_H_
