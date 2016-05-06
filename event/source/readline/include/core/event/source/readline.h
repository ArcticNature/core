// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_EVENT_SOURCE_READLINE_H_
#define CORE_EVENT_SOURCE_READLINE_H_

#include <memory>
#include <string>

#include "core/model/event.h"

namespace sf {
namespace core {
namespace event {

  //! Type of functions that convert a line to an event.
  typedef sf::core::model::EventRef (*LineParser)(std::string line);

  //! Event source that reads lines using GNU readline.
  class ReadlineEventSource : public sf::core::model::EventSource {
   protected:
    bool done;      //!< True when readline returns nullptr.
    bool parsed;    //!< True when line is read, reset each parse.
    int  input_fd;  //!< Duplicate fd for stdin.

    //! Last (full) line read.
    std::string buffer;

    //! Function to parse lines read from stdin.
    LineParser _parser;

    explicit ReadlineEventSource(LineParser parser);
    bool hasMoreData();

   public:
    virtual ~ReadlineEventSource();

    int getFD();
    sf::core::model::EventRef parse();

   protected:
    static sf::core::model::EventSourceRef _instance;
    static ReadlineEventSource* _self;
    static void handleLine(char* line);

   public:
    static void destroy();
    static sf::core::model::EventSourceRef instance(LineParser parser);
  };

}  // namespace event
}  // namespace core
}  // namespace sf

#endif  // CORE_EVENT_SOURCE_READLINE_H_
