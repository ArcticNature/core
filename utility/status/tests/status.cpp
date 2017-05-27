// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>

#include "core/utility/status.h"


using sf::core::utility::Status;
using sf::core::utility::StatusLight;
using sf::core::utility::SubsystemStatus;


TEST(Status, Colour) {
  Status status(StatusLight::YELLOW, "test");
  ASSERT_EQ(StatusLight::YELLOW, status.colour());
}

TEST(Status, Message) {
  Status status(StatusLight::YELLOW, "test");
  ASSERT_EQ("test", status.message());
}


TEST(SubsystemStatus, Empty) {
  SubsystemStatus status;
  ASSERT_EQ(StatusLight::UNKOWN, status.colour());
  ASSERT_EQ("No subsystem available", status.message());
}

TEST(SubsystemStatus, Green) {
  SubsystemStatus status;
  status.set("sub1", Status(StatusLight::GREEN, "test1"));
  status.set("sub2", Status(StatusLight::GREEN, "test2"));
  ASSERT_EQ(StatusLight::GREEN, status.colour());
  ASSERT_EQ("All subsystems are healthy", status.message());
}

TEST(SubsystemStatus, GreenOrUnkown) {
  SubsystemStatus status;
  status.set("sub1", Status(StatusLight::GREEN, "test1"));
  status.set("sub2", Status(StatusLight::UNKOWN, "test2"));
  ASSERT_EQ(StatusLight::UNKOWN, status.colour());
  ASSERT_EQ("test2", status.message());
}

TEST(SubsystemStatus, Red) {
  SubsystemStatus status;
  status.set("sub1", Status(StatusLight::UNKOWN, "test1"));
  status.set("sub2", Status(StatusLight::GREEN, "test2"));
  status.set("sub3", Status(StatusLight::YELLOW, "test3"));
  status.set("sub4", Status(StatusLight::RED, "test4"));
  status.set("sub5", Status(StatusLight::RED, "test5"));
  ASSERT_EQ(StatusLight::RED, status.colour());
  ASSERT_EQ("test4", status.message());
}

TEST(SubsystemStatus, Yellow) {
  SubsystemStatus status;
  status.set("sub1", Status(StatusLight::UNKOWN, "test1"));
  status.set("sub2", Status(StatusLight::GREEN, "test2"));
  status.set("sub3", Status(StatusLight::YELLOW, "test3"));
  status.set("sub4", Status(StatusLight::YELLOW, "test4"));
  ASSERT_EQ(StatusLight::YELLOW, status.colour());
  ASSERT_EQ("test3", status.message());
}

TEST(SubsystemStatus, Update) {
  SubsystemStatus status;
  status.set("subsys", Status(StatusLight::GREEN, "test1"));
  status.set("subsys", Status(StatusLight::RED, "test2"));
  ASSERT_EQ(StatusLight::RED, status.colour());
  ASSERT_EQ("test2", status.message());
}
