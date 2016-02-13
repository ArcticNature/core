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


  //! Helper methods for I/O of messages.
  template<typename Message>
  class MessageIO {
   public:
    //! Reads and parses a message from a file descriptor.
    /*!
     * The length of the message is expected in front of the message itself.
     * 
     * \param fd      The file descriptor from which the message should be read.
     * \param message Pointer to the Message object that will store the parsed
     *                input.
     * \returns The result of the parsing process.
     */
    static bool parse(int fd, Message* message);

    //! Serialises and sends a message to a file descriptor.
    /*!
     * The message length is written in front of the message itself.
     *
     * \param fd      The file descriptor that should receive the message.
     * \param message The message to send.
     * \returns The result of the serialisation process.
     */
    static bool send(int fd, Message message);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf


#include "core/utility/protobuf.inc.h"

#endif  // CORE_UTILITY_PROTOBUF_H_
