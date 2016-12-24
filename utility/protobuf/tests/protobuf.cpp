// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/utility/protobuf.h"

#include "core/event/testing.h"
#include "core/posix/user.h"
#include "core/protocols/test/t_message.pb.h"

#define WRITE_BUFFER_SIZE 512


using sf::core::context::Static;
using sf::core::event::MockDrain;

using sf::core::model::EventDrainBuffer;
using sf::core::model::EventDrainBufferRef;
using sf::core::model::EventDrainRef;
using sf::core::utility::LengthIO;
using sf::core::utility::MessageIO;

using sf::core::posix::User;
using sf::core::protocol::test::Message;


class IOTest : public ::testing::Test {
 protected:
  EventDrainRef drain;
  int read_fd;
  int write_fd;

 public:
  IOTest() {
    Static::initialise(new User());
    MockDrain* mock = new MockDrain("test");
    this->drain = EventDrainRef(mock);

    this->read_fd  = mock->readFD();
    this->write_fd = mock->fd();
    fcntl(this->write_fd, F_SETPIPE_SZ, WRITE_BUFFER_SIZE);
  }

  ~IOTest() {
    this->drain.reset();
    Static::destroy();
  }
};


class LengthIOTest  : public IOTest {};
class MessageIOTest : public IOTest {};


TEST_F(LengthIOTest, Decode) {
  uint32_t value = htonl(4);
  Static::posix()->write(this->write_fd, &value, UINT32_SIZE);
  value = LengthIO::decode(this->read_fd);
  ASSERT_EQ(4, value);
}

TEST_F(LengthIOTest, Encode) {
  // Encode to buffer.
  EventDrainBufferRef buffer(new EventDrainBuffer(UINT32_SIZE));
  LengthIO::encode(buffer, 4);

  // Write to fd.
  uint32_t size;
  char* data = buffer->remaining(&size);
  Static::posix()->write(this->write_fd, data, size);

  // Decode it back.
  uint32_t value = 0;
  Static::posix()->read(this->read_fd, &value, UINT32_SIZE);
  ASSERT_EQ(4, ntohl(value));
}


TEST_F(MessageIOTest, WriteReadMessage) {
  Message parsed;
  Message sent;
  sent.set_code(Message::Test);

  // Send.
  bool result = MessageIO<Message>::send(this->drain, sent);
  this->drain->flush();
  ASSERT_TRUE(result);

  // Read.
  result = MessageIO<Message>::parse(this->read_fd, &parsed);
  ASSERT_TRUE(result);
  ASSERT_EQ(Message::Test, parsed.code());
}
