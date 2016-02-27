// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "core/context/context.h"
#include "core/context/static.h"

#include "core/event/drain/fd.h"
#include "core/event/source/fd.h"
#include "core/event/source/unix.h"

#include "core/model/event.h"
#include "core/posix/user.h"
#include "core/protocols/test/t_message.pb.h"

#include "core/utility/string.h"
#include "core/utility/protobuf.h"

#include "ext/event/manager/epoll.h"


#define SOCKET_FILE "out/build/test/unix.socket"


using sf::core::context::Context;
using sf::core::context::Static;

using sf::core::event::FdDrain;
using sf::core::event::FdSource;
using sf::core::event::UnixSource;

using sf::core::model::Event;
using sf::core::model::EventDrainManager;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;
using sf::core::model::EventSourceManagerRef;
using sf::core::model::EventSourceRef;

using sf::core::posix::User;
using sf::core::protocol::test::Message;

using sf::core::utility::MessageIO;
using sf::core::utility::string::toString;

using sf::ext::event::EpollSourceManager;


class TestEvent : public Event {
 public:
  TestEvent(std::string correlation) : Event(correlation, "NULL") {}
  void handle() {}
};


class TestFdDrain : public FdDrain {
 public:
  TestFdDrain(int fd, std::string id) : FdDrain(fd, id) {}
  void sendAck() {}
};


class TestFdSource : public FdSource {
 public:
  TestFdSource(int fd, std::string id) : FdSource(fd, id) {}
  EventRef parse() {
    Message message;
    bool valid = MessageIO<Message>::parse(this->fd, &message);

    if (valid && message.code() == Message::Test) {
      return EventRef(new TestEvent("abc"));
    }
    return EventRef();
  }
};


class TestUnixSource : public UnixSource {
 protected:
  EventDrainRef clientDrain(int fd, std::string id) {
    return EventDrainRef(new TestFdDrain(fd, id));
  }

  EventSourceRef clientSource(int fd, std::string id) {
    return EventSourceRef(new TestFdSource(fd, id));
  }

 public:
  TestUnixSource(std::string path, std::string id) : UnixSource(path, id) {}
};


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

    int result = Static::posix()->connect(
        this->client_fd, (struct sockaddr*)&remote,
        sizeof(sockaddr_un)
    );

    EXPECT_EQ(0, result);
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
  TestUnixSource source(SOCKET_FILE, "id");
  int fd = source.getFD();

  ASSERT_NE(-1, fd);
  ASSERT_TRUE(this->fileExists(SOCKET_FILE));
}

TEST_F(UnixSourceTest, DeletesSockFile) {
  {
    TestUnixSource source(SOCKET_FILE, "id");
    int fd = source.getFD();
  }
  ASSERT_FALSE(this->fileExists(SOCKET_FILE));
}

TEST_F(UnixSourceTest, ConnectAddsSource) {
  EventSourceRef source(new TestUnixSource(SOCKET_FILE, "test"));
  EventSourceManagerRef sources = Context::sourceManager();

  int source_fd = source->getFD();
  sources->addSource(source);
  this->connect(source_fd);

  int remote_fd = this->client_fd + 1;
  std::string client_id = "fd-unix-test-client-" + toString(remote_fd);
  EventRef event = sources->wait(1);
  sources->removeSource(client_id);

  ASSERT_EQ(nullptr, event.get());
}

TEST_F(UnixSourceTest, ConnectAddsDrain) {
  EventDrainManager* drains = Static::drains();
  EventSourceRef source(new TestUnixSource(SOCKET_FILE, "test"));
  EventSourceManagerRef sources = Context::sourceManager();

  int source_fd = source->getFD();
  sources->addSource(source);
  this->connect(source_fd);

  int remote_fd = this->client_fd + 1;
  std::string client_id = "fd-unix-test-client-" + toString(remote_fd);
  EventRef event = sources->wait(1);
  drains->remove(client_id);

  ASSERT_EQ(nullptr, event.get());
}

TEST_F(UnixSourceTest, ClientToServer) {
  // Create UNIX source and connect client.
  EventSourceRef source(new TestUnixSource(SOCKET_FILE, "test"));
  EventSourceManagerRef sources = Context::sourceManager();

  int source_fd = source->getFD();
  sources->addSource(source);
  this->connect(source_fd);
  sources->wait(1);

  // Send message throug the client.
  Message message;
  message.set_code(Message::Test);
  bool sent = MessageIO<Message>::send(this->client_fd, message);
  ASSERT_TRUE(sent);

  // Check that the message is parsed from the source.
  EventRef event = sources->wait(10);
  ASSERT_NE(nullptr, event.get());
  ASSERT_NE(nullptr, dynamic_cast<TestEvent*>(event.get()));
}

TEST_F(UnixSourceTest, ServerToClient) {
  // Create UNIX source and connect client.
  EventSourceRef source(new TestUnixSource(SOCKET_FILE, "test"));
  EventSourceManagerRef sources = Context::sourceManager();

  int source_fd = source->getFD();
  sources->addSource(source);
  this->connect(source_fd);
  sources->wait(1);

  // Send message down the drain.
  int remote_fd = this->client_fd + 1;
  std::string client_id = "fd-unix-test-client-" + toString(remote_fd);
  EventDrainRef drain = Static::drains()->get(client_id);

  Message message;
  message.set_code(Message::Test);
  bool sent = MessageIO<Message>::send(drain->getFD(), message);
  ASSERT_TRUE(sent);

  // Check that the message is read from the client.
  Message recieved;
  bool valid = MessageIO<Message>::parse(this->client_fd, &recieved);
  ASSERT_TRUE(valid);
  ASSERT_EQ(Message::Test, recieved.code());
}
