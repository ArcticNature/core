// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_CLIENT_INPUT_H_
#define CORE_BIN_CLIENT_INPUT_H_

#include <ostream>
#include <string>

#include "core/model/event.h"

namespace sf {
namespace core {
namespace bin {

  //! Runs the given lua string in the client's environment.
  class ExecuteString : public sf::core::model::Event {
   protected:
    std::ostream* out;
    std::string   code;
    int line;

    void _handle();
    void printStack();

   public:
    ExecuteString(std::string code, int line, std::ostream* out = nullptr);
    void handle();
  };


  //! Enables the readline event source.
  class EnableReadline : public sf::core::model::Event {
   protected:
    static sf::core::model::EventRef parse(std::string line);

   public:
    EnableReadline();
    void handle();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_CLIENT_INPUT_H_
