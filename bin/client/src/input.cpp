// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/bin/client/input.h"

#include <iostream>
#include <ostream>
#include <string>

#include "core/context/client.h"
#include "core/context/context.h"

#include "core/event/source/readline.h"
#include "core/exceptions/lua.h"

#include "core/event/drain/null.h"
#include "core/model/event.h"

#include "core/lifecycle/event.h"
#include "core/utility/lua.h"
#include "core/utility/string.h"

using sf::core::bin::EnableReadline;
using sf::core::bin::ExecuteString;

using sf::core::context::Client;
using sf::core::context::Context;

using sf::core::event::NullDrain;
using sf::core::event::ReadlineEventSource;
using sf::core::exception::CleanExit;
using sf::core::exception::LuaSyntaxError;
using sf::core::exception::SfException;

using sf::core::lifecycle::EventLifecycle;
using sf::core::model::Event;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;

using sf::core::utility::Lua;
using sf::core::utility::string::toString;


void ExecuteString::_handle() {
  std::string name = "stdin:" + toString(this->line);
  std::string top_exp = "return " + this->code;
  Lua lua = Client::lua();

  try {
    lua.doString(top_exp, name);
  } catch (LuaSyntaxError&) {
    lua.stack()->clear();
    lua.doString(this->code, name);
  }

  this->printStack();
}

void ExecuteString::printStack() {
  Lua lua = Client::lua();
  lua.stack()->print(this->out);
}


ExecuteString::ExecuteString(
    std::string code, int line, std::ostream* out
) : Event("stdin:" + toString(line), EventDrainRef(new NullDrain())) {
  this->out  = out;
  this->code = code;
  this->line = line;

  if (this->out == nullptr) {
    this->out = &std::cout;
  }
}

void ExecuteString::handle() {
  ReadlineEventSource::clearLine();
  try {
    this->_handle();
    ReadlineEventSource::showPrompt();

  } catch (LuaSyntaxError& ex) {
    this->printStack();
    ReadlineEventSource::showPrompt();

  } catch (CleanExit& ex) {
    throw;

  } catch (SfException& ex) {
    std::cerr << ex.what() << std::endl;
    ReadlineEventSource::showPrompt();

  } catch (...) {
    ReadlineEventSource::showPrompt();
    throw;
  }
}


EventRef EnableReadline::parse(std::string line) {
  if (line == "") {
    return EventRef();
  }

  // Keep track of the line number being read.
  static unsigned int line_num = 0;
  line_num += 1;

  // Create the event to execute the line.
  EventRef exec = EventLifecycle::make<ExecuteString>(line, line_num);
  return exec;
}


EnableReadline::EnableReadline() : Event(
    "enable-readline", EventDrainRef(new NullDrain())
) {
  // NOOP.
}

void EnableReadline::handle() {
  Context::LoopManager()->add(
      ReadlineEventSource::instance(EnableReadline::parse)
  );
}
