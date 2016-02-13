// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/protobuf.h"

#include <arpa/inet.h>

#include "core/exceptions/base.h"
#include "core/context/static.h"


using sf::core::exception::CorruptedData;
using sf::core::exception::ChannelEmpty;

using sf::core::context::Static;
using sf::core::utility::LengthIO;


uint32_t LengthIO::read(int fd) {
  uint32_t value = 0;
  size_t   read = Static::posix()->read(fd, &value, sizeof(uint32_t));

  if (read != sizeof(uint32_t)) {
    throw CorruptedData("Invalid message length read.");
  }
  if (read == 0) {
    throw ChannelEmpty();
  }

  return ntohl(value);
}

void LengthIO::write(int fd, uint32_t length) {
  length = htonl(length);
  Static::posix()->write(fd, &length, sizeof(uint32_t));
}
