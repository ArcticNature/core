// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_EVENT_H_
#define CORE_MODEL_EVENT_H_

#include <exception>
#include <map>
#include <memory>
#include <string>

#include "core/exceptions/base.h"
#include "core/exceptions/event.h"
#include "core/interface/lifecycle.h"
#include "core/utility/lookup-table.h"


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
   * cooperatively react to the user or the system) and a drain (to
   * know where to send the result of an operation when done).
   */
  class Event {
   protected:
    std::string correlation_id;
    std::string event_id;
    EventDrainRef _drain;

   public:
    Event(std::string correlation, EventDrainRef drain);
    virtual ~Event();

    //! Returns the event correlation id.
    std::string correlation() const;

    //! Returns the event drain associated to this event.
    EventDrainRef drain() const;

    //! Initialises the event id, if missing.
    std::string id(std::string id);

    //! Returns the event id.
    std::string id() const;

    //! Handles the event in the most appropriate way.
    virtual void handle() = 0;

    //! Attempt to rescure form an handling error.
    virtual void rescue(std::exception_ptr ex);
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
    std::vector<std::string> buffer;

   public:
    explicit EventDrain(std::string id);
    virtual ~EventDrain();

    //! Enqueues a chunk of data to send.
    /*!
     * The chunk to send is stored in a string.
     *
     * The string instance is stored intentally until
     * it is flushed to the file descriptor.
     *
     * When sending ProtBuf messages use SerializeToString:
     * https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.message_lite#MessageLite.SerializeToString.details
     */
    virtual void enqueue(const std::string& chunk);

    //! \returns The writable file descriptor to send data to.
    virtual int fd() = 0;

    //! Flushes the internal buffer.
    /*!
     * Uses the internal `send` operator to perform the write.
     *
     * \returns true if the buffer is empty after the operation.
     */
    virtual bool flush() = 0;

    //! \returns the ID of the event drain.
    std::string id() const;

    //! Handles exceptions thrown while enqueuing or flushing.
    /**
     * When an exception is thrown while enqueuing data to send or
     * while flushing data, this method is called with that exception.
     * 
     * It is possible to throw exceptions from within this method
     * but these are not caught.
     */
    virtual void rescue(std::exception_ptr ex);

    //! Returns a Promise that is resolved/rejected when the drain is closed.
    // TODO(stefano): Promise whenClosed() const;
  };


  //! Manages a set of event drains.
  /*!
   * This is usefull to make drains available to other parts of the system.
   *
   * Examples are:
   *   - The spanwer/manager/daemon drains.
   *   - The HttpRequest drain.
   */
  typedef sf::core::utility::LookupTable<
    EventDrainRef,
    sf::core::exception::DuplicateEventDrain,
    sf::core::exception::EventDrainNotFound
  > EventDrainManager;
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

    //! Parses an event from the source.
    /*!
     * Called when the EventSourceManager determines that the source is ready
     * for reading.
     * \returns an Event to handle or nullptr if no event needs handling.
     */
    virtual EventRef parse() = 0;

   public:
    explicit EventSource(std::string id);
    virtual ~EventSource();

    //! \returns the ID of the event source.
    std::string id() const;

    //! \returns the file descriptor to wait for events on.
    virtual int fd() = 0;

    //! Fetches an event from the channel.
    /*!
     * When the source is ready to produce an event,
     * this method will call the `parse` method and deal with
     * errors if needed and possible.
     */
    EventRef fetch();

    //! Attempt to handle a error during parsing.
    virtual void rescue(std::exception_ptr ex);

    //! Returns a Promise that is resolved/rejected when the drain is closed.
    // TODO(stefano): Promise whenClosed() const;
  };
  typedef std::shared_ptr<EventSource> EventSourceRef;


  //! Store Source and Drain references by id and fd.
  template<typename Ref>
  class RefStore {
   protected:
    std::map<std::string, Ref> by_id;
    std::map<int, Ref> by_fd;

   public:
    //! Adds a Drain or Source reference to the store.
    void add(Ref ref);

    //! Returns the reference with the given fd/id.
    Ref  get(int fd) const;
    Ref  get(std::string id) const;

    //! Removes the reference with the given fd/id.
    void remove(int fd);
    void remove(std::string id);
  };


  //! Manages event sources and drains.
  /**
   * As the system can receive events from multiple sources and send data to
   * multiple drains, a LoopManager is responsible for coordinating the
   * reading and handling of these events as well as the flushing of drains.
   *
   * The LoopManager instance in use is stored in the Context so there is only
   * one **active** LoopManager (but there could be multiple LoopManager
   * instances existing in the system).
   */
  class LoopManager {
   protected:
    RefStore<EventDrainRef>  drains;
    RefStore<EventSourceRef> sources;

   public:
    virtual ~LoopManager();

    //! Returns a pointer to the requested event source.
    template<typename EventSrc>
    EventSrc* downcast(std::string id) const;

    //! Adds an event drain to the manager.
    virtual void add(EventDrainRef drain) = 0;

    //! Adds an event source to the manager.
    virtual void add(EventSourceRef source) = 0;

    //! Returns en event source reference for the requested source.
    EventSourceRef fetch(std::string id) const;

    //! Removes an event drain from the manager.
    virtual void removeDrain(std::string id) = 0;

    //! Removes an event source from the manager.
    virtual void removeSource(std::string id) = 0;

    //! Waits for one or more file descriptors to be ready.
    /**
     * The method blocks the execution of the current thread until
     * an event is returned or until timeout expires.
     *
     * \param timeout How long (in ms) to block for before giving up.
     * The default of -1 will block indefinitely.
     */
    virtual EventRef wait(int timeout = -1) = 0;
  };
  typedef std::shared_ptr<LoopManager> LoopManagerRef;


  //! TODO(stefano)
  class BoundDrain : public EventDrain {
    // TODO(stefano): void bind(EventSourceRef source);
  };


  //! TODO(stefano)
  class BoundSource : public EventSource {
    // TODO(stefano): void bind(EventDrainRef drain);
  };

}  // namespace model
}  // namespace core
}  // namespace sf

namespace sf {
namespace core {
namespace lifecycle {

  const std::string EVENT_DRAIN_ENQUEUE = "event::drain::enqueue";

  //! Arguments to `event::drain::enqueue`.
  class DrainEnqueueArg : public sf::core::interface::BaseLifecycleArg {
   protected:
    bool _added;
    std::string _drain;

   public:
    explicit DrainEnqueueArg(std::string drain);

    //! Marks the drain as added to the loop manager.
    void add();

    //! Checks if the drain was added to the loop manager.
    bool added() const;

    //! Return the event drain where data was enqueued.
    std::string drain() const;
  };


  //! Lifecycle handler for `event::drain:enqueue`.
  typedef sf::core::interface::LifecycleHandler<DrainEnqueueArg>
    DrainEnqueueHandler;

}  // namespace lifecycle
}  // namespace core
}  // namespace sf

#include "core/model/event.inc.h"

#endif  // CORE_MODEL_EVENT_H_
