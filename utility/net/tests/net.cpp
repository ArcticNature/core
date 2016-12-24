// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/utility/net.h"
#include "core/posix/user.h"

#define SOCKET_PATH "./core.utility.net.test"

using sf::core::context::Static;

using sf::core::utility::FileDescriptorState;
using sf::core::utility::checkFdState;

using sf::core::posix::User;


class CheckFdTest : public ::testing::Test {
 protected:
  int listen_fd;
  int client_fd;
  int server_fd;

 public:
  CheckFdTest() {
    Static::initialise(new User());

    // Listen to a connection.
    struct sockaddr_un local;
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, SOCKET_PATH, sizeof(local.sun_path));
    this->listen_fd = Static::posix()->socket(
        AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0
    );
    Static::posix()->bind(
        this->listen_fd, (struct sockaddr*)&local,
        sizeof(sockaddr_un)
    );
    Static::posix()->listen(this->listen_fd, 1);

    // Connect from the "client" side.
    this->client_fd = Static::posix()->socket(
        AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0
    );
    Static::posix()->connect(
        this->client_fd, (struct sockaddr*)&local,
        sizeof(sockaddr_un)
    );

    // Accept from the "server" side.
    this->server_fd = Static::posix()->accept(
        this->listen_fd, nullptr, nullptr,
        SOCK_NONBLOCK | SOCK_CLOEXEC
    );
  }

  ~CheckFdTest() {
    // Close all FDs.
    if (this->client_fd != -1) {
      Static::posix()->close(this->client_fd);
    }
    Static::posix()->close(this->server_fd);
    Static::posix()->close(this->listen_fd);

    // And delete file if needed.
    struct stat stat_info;
    if (Static::posix()->stat(SOCKET_PATH, &stat_info) == 0) {
      Static::posix()->unlink(SOCKET_PATH);
    }
    Static::destroy();
  }

  void closeClient() {
    Static::posix()->close(this->client_fd);
    this->client_fd = -1;
  }

  std::string read() {
    char buffer[256];
    Static::posix()->read(this->client_fd, buffer, 256);
    return std::string(buffer);
  }

  void write(std::string buffer) {
    Static::posix()->write(this->server_fd, buffer.c_str(), buffer.length());
  }
};

TEST_F(CheckFdTest, CheckDoesNotReadStream) {
  this->write("hello");
  FileDescriptorState state = checkFdState(this->server_fd);
  std::string payload = this->read();

  ASSERT_EQ(FileDescriptorState::OK, state);
  ASSERT_EQ("hello", payload);
}

TEST_F(CheckFdTest, DetectClosed) {
  this->closeClient();
  FileDescriptorState state = checkFdState(this->server_fd);
  ASSERT_EQ(FileDescriptorState::CLOSED, state);
}

TEST_F(CheckFdTest, DetectOpen) {
  FileDescriptorState state = checkFdState(this->server_fd);
  ASSERT_EQ(FileDescriptorState::OK, state);
}
