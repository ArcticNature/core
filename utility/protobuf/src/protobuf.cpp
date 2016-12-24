// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/protobuf.h"

#include <arpa/inet.h>

#include "core/model/event.h"
#include "core/exceptions/base.h"
#include "core/context/static.h"


using sf::core::exception::CorruptedData;
using sf::core::exception::ChannelEmpty;

using sf::core::context::Static;
using sf::core::model::EventDrainBufferRef;
using sf::core::utility::LengthIO;


uint32_t LengthIO::decode(int fd) {
  uint32_t value = 0;
  size_t   read = Static::posix()->read(fd, &value, UINT32_SIZE);

  if (read != UINT32_SIZE) {
    throw CorruptedData("Invalid message length read.");
  }
  if (read == 0) {
    throw ChannelEmpty();
  }

  return ntohl(value);
}

void LengthIO::encode(EventDrainBufferRef buffer, uint32_t length) {
  length = htonl(length);
  uint32_t* start = static_cast<uint32_t*>(buffer->data(0));
  *start = length;
}
