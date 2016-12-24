// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_PROTOBUF_INC_H_
#define CORE_UTILITY_PROTOBUF_INC_H_

#include <iostream>

#include "core/model/event.h"
#include "core/utility/fdstream.h"

namespace sf {
namespace core {
namespace utility {

  template<typename Message>
  bool MessageIO<Message>::send(
      sf::core::model::EventDrainRef drain, Message message
  ) {
    uint32_t msg_size = message.ByteSize();
    uint32_t size = msg_size + UINT32_SIZE;
    sf::core::model::EventDrainBufferRef buffer(
        new sf::core::model::EventDrainBuffer(size)
    );

    LengthIO::encode(buffer, msg_size);
    void* data = buffer->data(UINT32_SIZE);
    if (!message.SerializeToArray(data, msg_size)) {
      // TODO(stefano): throw an exception.
      return false;
    }

    drain->enqueue(buffer);
    return true;
  }

  template<typename Message>
  bool MessageIO<Message>::parse(int fd, Message* message) {
    uint32_t length = LengthIO::decode(fd);
    FdStreamBuf  istream_buffer(fd, length);
    std::istream input(&istream_buffer);
    return message->ParseFromIstream(&input);
  }

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_PROTOBUF_INC_H_
