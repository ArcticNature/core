// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include <fcntl.h>
#include <unistd.h>

#include <gtest/gtest.h>

#include <string>

#include "core/context/static.h"
#include "core/posix/user.h"
#include "core/utility/fdstream.h"

#define WRITE_BUFFER_SIZE 512


using sf::core::context::Static;
using sf::core::posix::User;
using sf::core::utility::FdStreamBuf;


class FdStreamBufTest : public ::testing::Test {
 protected:
  int read_fd;
  int write_fd;

  void writeString(std::string str) {
    if (str.length() > WRITE_BUFFER_SIZE) {
      throw std::invalid_argument("String too long");
    }
    Static::posix()->write(this->write_fd, str.c_str(), str.length());
  }

 public:
  FdStreamBufTest() {
    Static::initialise(new User());

    int pipe[2];
    Static::posix()->pipe(pipe, O_NONBLOCK);
    fcntl(this->write_fd, F_SETPIPE_SZ, WRITE_BUFFER_SIZE);
    this->read_fd  = pipe[0];
    this->write_fd = pipe[1];
  }

  ~FdStreamBufTest() {
    Static::posix()->close(this->read_fd);
    Static::posix()->close(this->write_fd);
    Static::reset();
  }
};


TEST_F(FdStreamBufTest, ReadEmpty) {
  FdStreamBuf  streamBuffer(this->read_fd);
  std::istream stream(&streamBuffer);
  this->writeString("");
  stream.get();
  ASSERT_TRUE(stream.eof());
}

TEST_F(FdStreamBufTest, ReadInt) {
  FdStreamBuf  streamBuffer(this->read_fd);
  std::istream stream(&streamBuffer);
  this->writeString("4242");

  int number;
  stream >> number;
  ASSERT_EQ(number, 4242);

  stream.get();
  ASSERT_TRUE(stream.eof());
}

TEST_F(FdStreamBufTest, ReadOne) {
  FdStreamBuf  streamBuffer(this->read_fd);
  std::istream stream(&streamBuffer);
  this->writeString("a");

  char ch = (char)stream.get();
  ASSERT_EQ(ch, 'a');

  stream.get();
  ASSERT_TRUE(stream.eof());
}

TEST_F(FdStreamBufTest, ReadPastBuffer) {
  FdStreamBuf  streamBuffer(this->read_fd, FdStreamBuf::READ_ALL, 2);
  std::istream stream(&streamBuffer);
  this->writeString("abcdef");

  std::string text;
  stream >> text;
  ASSERT_EQ(text, "abcdef");

  stream.get();
  ASSERT_TRUE(stream.eof());
}

TEST_F(FdStreamBufTest, ReadBackWithinBufferBegin) {
  FdStreamBuf  streamBuffer(this->read_fd);
  std::istream stream(&streamBuffer);
  this->writeString("abcdef");

  std::string text;
  stream >> text;
  stream.seekg(2, stream.beg);

  text = "";
  stream >> text;
  ASSERT_EQ(text, "cdef");

  stream.get();
  ASSERT_TRUE(stream.eof());
}

TEST_F(FdStreamBufTest, ReadBackWithinBufferEnd) {
  FdStreamBuf  streamBuffer(this->read_fd);
  std::istream stream(&streamBuffer);
  this->writeString("abcdef");

  std::string text;
  stream >> text;
  stream.seekg(-2, stream.end);

  text = "";
  stream >> text;
  ASSERT_EQ(text, "ef");

  stream.get();
  ASSERT_TRUE(stream.eof());
}

TEST_F(FdStreamBufTest, ReadBackWithinBufferOffset) {
  FdStreamBuf  streamBuffer(this->read_fd);
  std::istream stream(&streamBuffer);
  this->writeString("abcdef");

  std::string text;
  stream >> text;
  stream.seekg(-4, stream.cur);

  text = "";
  stream >> text;
  ASSERT_EQ(text, "cdef");

  stream.get();
  ASSERT_TRUE(stream.eof());
}

TEST_F(FdStreamBufTest, LimitedRead) {
  FdStreamBuf  streamBuffer(this->read_fd, 2);
  std::istream stream(&streamBuffer);
  this->writeString("1234");

  int number;
  stream >> number;
  stream.get();

  ASSERT_EQ(12, number);
  ASSERT_TRUE(stream.eof());
}
