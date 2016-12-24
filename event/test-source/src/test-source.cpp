// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/test-source.h"

#include <string>

#include "core/context/static.h"
#include "core/event/testing.h"
#include "core/model/event.h"

#include "core/protocols/test/t_message.pb.h"
#include "core/utility/protobuf.h"


using sf::core::context::Static;
using sf::core::event::TestFdSource;

using sf::core::model::EventDrain;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSource;
using sf::core::model::EventSourceRef;

using sf::core::protocol::test::Message;
using sf::core::utility::MessageIO;


class NullDrain : public EventDrain {
 public:
  NullDrain() : EventDrain("NULL") {
    // Noop.
  }

  int fd() {
    return -1;
  }

  bool flush() {
    return true;
  }
};




TestFdSource::TestFdSource(int fd, std::string id) : EventSource("fd-" + id) {
  this->_fd = fd;
}

TestFdSource::~TestFdSource() {
  Static::posix()->shutdown(this->_fd, SHUT_RD);
  Static::posix()->close(this->_fd);
}

int TestFdSource::fd() {
  return this->_fd;
}

EventRef TestFdSource::parse() {
  Message message;
  bool valid = MessageIO<Message>::parse(this->_fd, &message);
  if (valid && message.code() == Message::Test) {
    return EventRef(new TestEvent(
          "abc", EventDrainRef(new NullDrain())
    ));
  }
  return EventRef();
}
