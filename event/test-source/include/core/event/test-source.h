// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_TEST_SOURCE_H_
#define CORE_EVENT_TEST_SOURCE_H_

#include <map>
#include <string>

#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  class TestFdSource : public sf::core::model::EventSource {
   protected:
    int _fd;
    sf::core::model::EventRef parse();

   public:
    TestFdSource(int fd, std::string id);
    ~TestFdSource();
    int fd();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_TEST_SOURCE_H_
