// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <fcntl.h>
#include <gtest/gtest.h>

#include "core/bin/manager/api/source.h"
#include "core/context/static.h"
#include "core/exceptions/base.h"

#include "core/model/event.h"
#include "core/protocols/public/p_message.pb.h"
#include "core/utility/protobuf.h"

#include "core/posix/user.h"

using sf::core::bin::ApiFdSource;
using sf::core::context::Static;

using sf::core::exception::CorruptedData;
using sf::core::exception::FactoryNotFound;

using sf::core::model::EventRef;
using sf::core::protocol::public_api::Message;
using sf::core::utility::MessageIO;

using sf::core::posix::User;


class ApiSourceTest : public ::testing::Test {
 protected:
  int read_fd;
  int write_fd;

  ApiFdSource* api;

 public:
  ApiSourceTest() {
    Static::initialise(new User());

    int pipe[2];
    Static::posix()->pipe(pipe, O_NONBLOCK);
    this->read_fd  = pipe[0];
    this->write_fd = pipe[1];

    this->api = new ApiFdSource(this->read_fd, "test", "NULL");
  }

  ~ApiSourceTest() {
    delete this->api;
    Static::posix()->close(this->write_fd);
    Static::destroy();
  }

  void write(const char* buffer) {
    Static::posix()->write(this->write_fd, buffer, strlen(buffer));
  }

  void write(Message message) {
    MessageIO<Message>::send(this->write_fd, message);
  }
};


TEST_F(ApiSourceTest, FailCorruptData) {
  this->write("");
  ASSERT_THROW(this->api->parse(), CorruptedData);
}

TEST_F(ApiSourceTest, FailHandlerNotFound) {
  Message message;
  message.set_code(Message::Ack);
  this->write(message);
  ASSERT_THROW(this->api->parse(), FactoryNotFound);
}

TEST_F(ApiSourceTest, HandlerIsCalled) {
  Message message;
  message.set_code(Message::Introduce);
  this->write(message);
  EventRef event = this->api->parse();
  ASSERT_NE(nullptr, event.get());
}

TEST_F(ApiSourceTest, HasDrain) {
  Message message;
  message.set_code(Message::Introduce);
  this->write(message);
  EventRef event = this->api->parse();
  ASSERT_EQ("NULL", event->drain());
}
