// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_EVENT_FIXTURE_H_
#define CORE_MODEL_EVENT_FIXTURE_H_

#include <string>
#include "core/model/event.h"


namespace sf {
namespace testing {

  class FailEvent : public sf::core::model::Event {
   public:
    FailEvent(std::string cid);
    void handle();
  };


  class NoopEvent : public sf::core::model::Event {
   public:
    NoopEvent(std::string cid);
    NoopEvent(std::string cid, sf::core::model::EventDrainRef drain);
    void handle();
  };


  class TestDrain : public sf::core::model::EventDrain {
   public:
    TestDrain();
    int fd();
    bool flush();
    int length();
  };


  class TestSource : public sf::core::model::EventSource {
   protected:
    sf::core::model::EventRef parse();

   public:
    TestSource();
    int fd();
  };

}  // namespace testing
}  // namespace sf

#endif // CORE_MODEL_EVENT_FIXTURE_H_
