// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/utility/status.h"


using sf::core::utility::StatusDetail;
using sf::core::utility::StatusLight;
using sf::core::utility::SubsystemStatus;


enum TestCode {
  UNKOWN = -1,
  OK,
  OK_TWO,
  WARN,
  LOAD,
  ERROR,
  FAULT,
  END
};


class TestStatus : public StatusDetail<
    TestCode, TestCode::OK, TestCode::WARN,
    TestCode::ERROR, TestCode::END
> {
 public:
  TestStatus(TestCode code, std::string message) : StatusDetail(
      code, message
  ) {}
};


class TestSubsystem : public SubsystemStatus<TestStatus, TestCode> {
};


TEST(StatusDetail, Code) {
  TestStatus status(TestCode::LOAD, "test");
  ASSERT_EQ(TestCode::LOAD, status.code());
}

TEST(StatusDetail, ColourEnd) {
  TestStatus status(TestCode::END, "test");
  ASSERT_EQ(StatusLight::UNKOWN, status.colour());
}

TEST(StatusDetail, ColourGreen) {
  TestStatus status(TestCode::OK, "test");
  ASSERT_EQ(StatusLight::GREEN, status.colour());
}

TEST(StatusDetail, ColourRed) {
  TestStatus status(TestCode::ERROR, "test");
  ASSERT_EQ(StatusLight::RED, status.colour());
}

TEST(StatusDetail, ColourUnkown) {
  TestStatus status(TestCode::UNKOWN, "test");
  ASSERT_EQ(StatusLight::UNKOWN, status.colour());
}

TEST(StatusDetail, ColourYellow) {
  TestStatus status(TestCode::WARN, "test");
  ASSERT_EQ(StatusLight::YELLOW, status.colour());
}

TEST(StatusDetail, Message) {
  TestStatus status(TestCode::LOAD, "test");
  ASSERT_EQ("test", status.message());
}


TEST(SubsystemStatus, Empty) {
  TestSubsystem status;
  ASSERT_EQ(StatusLight::UNKOWN, status.colour());
  ASSERT_EQ(TestCode::UNKOWN, status.reason().code());
  ASSERT_EQ("no subsystem available", status.reason().message());
}

TEST(SubsystemStatus, HasItem) {
  TestSubsystem status;
  status.set("test", TestStatus(TestCode::OK, "test"));
  ASSERT_EQ(StatusLight::GREEN, status.colour());
  ASSERT_EQ(TestCode::OK, status.reason().code());
  ASSERT_EQ("test", status.reason().message());
}

TEST(SubsystemStatus, HasMultipleItems) {
  TestSubsystem status;
  status.set("warn", TestStatus(TestCode::WARN, "warn"));
  status.set("error", TestStatus(TestCode::ERROR, "error"));
  status.set("ok", TestStatus(TestCode::OK, "ok"));

  ASSERT_EQ(StatusLight::RED, status.colour());
  ASSERT_EQ(TestCode::ERROR, status.reason().code());
  ASSERT_EQ("error", status.reason().message());
}

TEST(SubsystemStatus, UnkownWinsOverOk) {
  TestSubsystem status;
  status.set("unkown", TestStatus(TestCode::UNKOWN, "unkown"));
  status.set("ok", TestStatus(TestCode::OK, "ok"));

  ASSERT_EQ(StatusLight::UNKOWN, status.colour());
  ASSERT_EQ(TestCode::UNKOWN, status.reason().code());
  ASSERT_EQ("unkown", status.reason().message());
}
