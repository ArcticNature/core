// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_PROTOBUF_H_
#define CORE_UTILITY_PROTOBUF_H_

#include <stdint.h>


namespace sf {
namespace core {
namespace utility {

  //! Helper methods for I/O of message lengths.
  class LengthIO {
   public:
    //! Reads a message length from the file descriptor.
    static uint32_t read(int fd);

    //! Writes a message length to the file descriptor.
    static void write(int fd, uint32_t length);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_PROTOBUF_H_
