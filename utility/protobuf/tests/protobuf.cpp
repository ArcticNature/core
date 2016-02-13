// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/utility/protobuf.h"

#include "core/posix/user.h"

#define WRITE_BUFFER_SIZE 512


using sf::core::context::Static;
using sf::core::utility::LengthIO;

using sf::core::posix::User;


class LengthIOTest : public ::testing::Test {
 protected:
  int read_fd;
  int write_fd;

 public:
  LengthIOTest() {
    Static::initialise(new User());

    int pipe[2];
    Static::posix()->pipe(pipe, O_NONBLOCK);
    fcntl(this->write_fd, F_SETPIPE_SZ, WRITE_BUFFER_SIZE);
    this->read_fd  = pipe[0];
    this->write_fd = pipe[1];
  }

  ~LengthIOTest() {
    Static::posix()->close(this->read_fd);
    Static::posix()->close(this->write_fd);
    Static::reset();
  }
};


TEST_F(LengthIOTest, Read) {
  uint32_t value = htonl(4);
  Static::posix()->write(this->write_fd, &value, sizeof(uint32_t));
  value = LengthIO::read(this->read_fd);
  ASSERT_EQ(4, value);
}

TEST_F(LengthIOTest, Write) {
  LengthIO::write(this->write_fd, 4);
  uint32_t value = 0;
  Static::posix()->read(this->read_fd, &value, sizeof(uint32_t));
  ASSERT_EQ(4, ntohl(value));
}
