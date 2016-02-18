// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/context/static.h"
#include "core/event/source/unix.h"
#include "core/posix/user.h"


#define SOCKET_FILE "out/build/test/unix.socket"


using sf::core::context::Static;
using sf::core::event::UnixSource;
using sf::core::posix::User;


class UnixSourceTest : public ::testing::Test {
 protected:
  bool fileExists(std::string path) {
    struct stat stat_info;
    return Static::posix()->stat(path.c_str(), &stat_info) == 0;
  }

 public:
  UnixSourceTest() {
    Static::initialise(new User());

    // Ensure that the socket file does not exist.
    if (this->fileExists(SOCKET_FILE)) {
      Static::posix()->unlink(SOCKET_FILE);
    }
  }

  ~UnixSourceTest() {
    Static::destroy();
  }
};


TEST_F(UnixSourceTest, CreatesSockFile) {
  UnixSource source(SOCKET_FILE, "id");
  int fd = source.getFD();

  ASSERT_NE(-1, fd);
  ASSERT_TRUE(this->fileExists(SOCKET_FILE));
}

TEST_F(UnixSourceTest, DeletesSockFile) {
  {
    UnixSource source(SOCKET_FILE, "id");
    int fd = source.getFD();
  }
  ASSERT_FALSE(this->fileExists(SOCKET_FILE));
}
