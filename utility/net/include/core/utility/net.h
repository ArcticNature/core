// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_NET_H_
#define CORE_UTILITY_NET_H_

namespace sf {
namespace core {
namespace utility {

  //! Indicates the current state of a file descriptor.
  enum FileDescriptorState {
    UNKOWN = -1,
    OK = 0,
    CLOSED = 1,
    ERROR = 2,
    NOT_SOCKET = 3
  };

  //! Check the state of a file descriptor.
  /*!
   * *This is reliable only for sockets*.
   * Checks if the file descriptor is open, closed,
   * or in and error state.
   *
   * Sockets are checked both locally and remotely for the
   * closed state.
   */
  FileDescriptorState checkFdState(int fd);

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_NET_H_
