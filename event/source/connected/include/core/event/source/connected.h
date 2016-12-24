// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_SOURCE_CONNECTED_H_
#define CORE_EVENT_SOURCE_CONNECTED_H_

#include <string>
#include <vector>

#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  //! Connection based event source.
  /*!
   * Connection based event sources are sources that linked to a drain.
   * When the source is closed, the drain should also be closed
   * and removed from the drain manager.
   */
  class ConnectedSource : public sf::core::model::EventSource {
   protected:
    const sf::core::model::EventDrainRef drain;
    bool _closed;

    //! Checks the source's file descriptor for closure or error.
    /*!
     * Useful to check the state of the FD before it is used by
     * the source to read data.
     *
     * The state of the file descriptor determines what this
     * method does:
     *
     *   - If the fd was closed, the `onclosed` method is called.
     *   - If the fd is in an error state, the `onerror` method is called.
     *   - If the fd is in an unkown state, the `onerror` method is called.
     *
     * \returns true only if the FD is in a usable state.
     */
    bool checkFD();

    //! Processes a closed connection.
    /*!
     * When a connection is closed by the client:
     *
     *   - The server side of the connection is closed.
     *   - The source is removed from the current source manager.
     *   - The drain is removed from the current drain manager.
     */
    virtual void onclose();

    //! Processes an errored connection.
    /*!
     * When a connection enters an error state:
     *
     *   - Information about the error is logged.
     *   - The source is removed from the current source manager.
     *   - The drain is removed from the current drain manager.
     */
    virtual void onerror();

    //! Terminates the connection to the client.
    virtual void disconnect() = 0;

    //! Removes the associated drain from the drain manager.
    void removeDrain();

    //! Removes the source from the drain manager.
    void removeSource();

    //! Cleans up the source, ignoring errors.
    /*!
     * \param self Remove yourself from the drain manager.
     */
    void safeCleanUp(bool self = false);

   public:
    ConnectedSource(std::string id, sf::core::model::EventDrainRef drain);
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_SOURCE_CONNECTED_H_
