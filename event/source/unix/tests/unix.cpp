// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "core/context/context.h"
#include "core/context/static.h"
#include "core/event/source/unix.h"
#include "core/model/event.h"

#include "core/posix/user.h"
#include "core/utility/string.h"

#include "ext/event/manager/epoll.h"


#define SOCKET_FILE "out/build/test/unix.socket"


using sf::core::context::Context;
using sf::core::context::Static;
using sf::core::event::UnixSource;

using sf::core::model::EventRef;
using sf::core::model::EventSourceManagerRef;
using sf::core::model::EventSourceRef;

using sf::core::posix::User;
using sf::core::utility::string::toString;

using sf::ext::event::EpollSourceManager;


class UnixSourceTest : public ::testing::Test {
 protected:
  int client_fd;

  bool fileExists(std::string path) {
    struct stat stat_info;
    return Static::posix()->stat(path.c_str(), &stat_info) == 0;
  }

  void connect(int to) {
    this->client_fd = Static::posix()->socket(
        AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0
    );
    
    struct sockaddr_un remote;
    remote.sun_family = AF_UNIX;
    strncpy(remote.sun_path, SOCKET_FILE, sizeof(remote.sun_path));
    Static::posix()->connect(
        this->client_fd, (struct sockaddr*)&remote,
        sizeof(sockaddr_un)
    );
  }

 public:
  UnixSourceTest() {
    this->client_fd = -1;

    Static::initialise(new User());
    Context::instance()->initialise(EventSourceManagerRef(
        new EpollSourceManager()
    ));

    // Ensure that the socket file does not exist.
    if (this->fileExists(SOCKET_FILE)) {
      Static::posix()->unlink(SOCKET_FILE);
    }
  }

  ~UnixSourceTest() {
    if (this->client_fd != -1) {
      Static::posix()->close(this->client_fd);
    }
    Context::destroy();
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

TEST_F(UnixSourceTest, ConnectAddsSource) {
  EventSourceRef source(new UnixSource(SOCKET_FILE, "id"));
  EventSourceManagerRef sources = Context::sourceManager();

  int fd = source->getFD();
  sources->addSource(source);
  this->connect(fd);

  std::string client_id = "unix-client-" + toString(this->client_fd);
  EventRef event = sources->wait(1);
  sources->removeSource(client_id);

  ASSERT_EQ(nullptr, event.get());
}


// Connect adds new drain.
