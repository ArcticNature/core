// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/net.h"

#include <fcntl.h>

#include "core/context/static.h"
#include "core/exceptions/base.h"

using sf::core::context::Static;
using sf::core::exception::ErrNoException;
using sf::core::utility::FileDescriptorState;


FileDescriptorState sf::core::utility::checkFdState(int fd) {
  try {
    char ch;
    int  read = Static::posix()->read(fd, &ch, 0);

    if (read == 0) {
      ssize_t size = Static::posix()->recv(
          fd, &ch, 1, MSG_DONTWAIT | MSG_PEEK
      );
      if (size == 0) {
        return FileDescriptorState::CLOSED;
      }
      return FileDescriptorState::OK;
    }

    return FileDescriptorState::UNKOWN;

  } catch (ErrNoException& ex) {
    switch (ex.getCode()) {
      case EAGAIN:   return FileDescriptorState::OK;
      case EBADF:    return FileDescriptorState::CLOSED;
      case ENOTSOCK: return FileDescriptorState::NOT_SOCKET;
    }
    return FileDescriptorState::UNKOWN;
  }
}


