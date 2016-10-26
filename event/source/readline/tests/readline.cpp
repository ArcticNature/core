// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <fcntl.h>

#include <iostream>
#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/event/source/readline.h"

#include "core/exceptions/base.h"
#include "core/model/event.h"

#include "core/event/testing.h"
#include "core/posix/user.h"

using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::ReadlineEventSource;

using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceRef;
using sf::core::model::LoopManagerRef;

using sf::core::event::MockDrain;
using sf::core::event::TestEpollManager;
using sf::core::posix::User;


class LineEvent : public Event {
 public:
  std::string line;

  LineEvent(std::string line) : Event(
      "test", EventDrainRef(new MockDrain("NULL"))
  ) {
    this->line = line;
  }

  void handle() {
    // NOOP.
  }
};


EventRef test_readline_parser(std::string line) {
  return EventRef(new LineEvent(line));
}


class ReadlineSourceTest : public ::testing::Test {
 protected:
  int stdin_start;
  int stdin_read;
  int stdin_write;

  ReadlineSourceTest() {
    Static::initialise(new User());
    Context::Instance()->initialise(LoopManagerRef(new TestEpollManager()));

    // Redirect stdin from a pipe.
    int buff[2];
    pipe2(buff, O_NONBLOCK);
    this->stdin_start = dup(fileno(::stdin));
    this->stdin_read  = buff[0];
    this->stdin_write = buff[1];
    dup2(this->stdin_read, fileno(::stdin));

    // Create an instance for the tests.
    Static::options()->setString("client-id", "TEST");
    Context::LoopManager()->add(
        ReadlineEventSource::instance(test_readline_parser)
    );
  }

  ~ReadlineSourceTest() {
    ReadlineEventSource::destroy();
    Context::Destroy();
    Static::destroy();

    // Restore initial stdin.
    dup2(this->stdin_start, fileno(::stdin));
    close(this->stdin_start);
    close(this->stdin_read);
    if (this->stdin_write != -1) {
      close(this->stdin_write);
    }

    std::cout << std::endl;
  }

  void closeStdin() {
    close(this->stdin_write);
    this->stdin_write = -1;
  }

  void stdin(std::string buffer) {
    write(this->stdin_write, buffer.c_str(), buffer.length());
  }

  void ASSERT_LINE(std::string expected) {
    EventRef   actual = Context::LoopManager()->wait(1);
    LineEvent* event  = dynamic_cast<LineEvent*>(actual.get());
    ASSERT_NE(nullptr, event);
    ASSERT_EQ(expected, event->line);
  }
};


TEST_F(ReadlineSourceTest, FlushOnClose) {
  this->stdin("abc");
  this->closeStdin();
  ASSERT_LINE("abc");
}

TEST_F(ReadlineSourceTest, ReadOneLine) {
  this->stdin("abc\n");
  ASSERT_LINE("abc");
}

TEST_F(ReadlineSourceTest, ReadOneLineForLoop) {
  this->stdin("abc\ndef\n\n");
  this->closeStdin();
  ASSERT_LINE("abc");
  ASSERT_LINE("def");
  ASSERT_LINE("");
}
