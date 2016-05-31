// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_PROTOBUF_INC_H_
#define CORE_UTILITY_PROTOBUF_INC_H_

#include <iostream>

#include "core/utility/fdstream.h"

namespace sf {
namespace core {
namespace utility {

  template<typename Message>
  bool MessageIO<Message>::send(int fd, Message message) {
    LengthIO::write(fd, message.ByteSize());
    if (!message.SerializeToFileDescriptor(fd)) {
      // TODO(stefano): throw an exception.
      return false;
    }
    return true;
  }

  template<typename Message>
  bool MessageIO<Message>::parse(int fd, Message* message) {
    uint32_t length = LengthIO::read(fd);
    FdStreamBuf  istream_buffer(fd, length);
    std::istream input(&istream_buffer);
    return message->ParseFromIstream(&input);
  }

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_PROTOBUF_INC_H_
