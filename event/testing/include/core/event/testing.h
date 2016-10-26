// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_TESTING_H_
#define CORE_EVENT_TESTING_H_

#include <map>
#include <string>

#include "core/event/drain/fd.h"
#include "core/model/event.h"


namespace sf {
namespace core {
namespace event {

  class MockDrain : public sf::core::model::EventDrain {
   protected:
    int read_fd;
    int write_fd;

   public:
    explicit MockDrain(std::string id);
    ~MockDrain();
    int  fd();
    bool flush();
    int  readFD();
  };


  class TestEvent : public sf::core::model::Event {
   public:
    TestEvent();
    TestEvent(
        std::string correlation,
        sf::core::model::EventDrainRef drain
    );
    virtual void handle();
  };


  class TestEpollManager : public sf::core::model::LoopManager {
   protected:
    int epoll_fd;

   public:
    TestEpollManager();
    virtual ~TestEpollManager();

    void add(sf::core::model::EventDrainRef drain);
    void add(sf::core::model::EventSourceRef source);
    void removeDrain(std::string id);
    void removeSource(std::string id);
    sf::core::model::EventRef wait(int timeout = -1);
  };


  class TestFdDrain : public sf::core::event::FdDrain {
   public:
    TestFdDrain(int fd, std::string id);
  };


  class TestFdSource : public sf::core::model::EventSource {
   protected:
    int _fd;
    sf::core::model::EventRef parse();

   public:
    TestFdSource(int fd, std::string id);
    ~TestFdSource();
    int fd();
  };


  class TestUnixClient : public sf::core::model::EventSource {
   protected:
    int socket_fd;
    std::string path;

    void openSocket();
    sf::core::model::EventRef parse();

   public:
    explicit TestUnixClient(std::string socket);
    ~TestUnixClient();

    int fd();
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_TESTING_H_
