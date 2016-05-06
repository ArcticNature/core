// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/readline.h"

#include <poll.h>
#include <string>

#include "readline/readline.h"
#include "core/context/static.h"

using sf::core::context::Static;
using sf::core::event::ReadlineEventSource;

using sf::core::model::EventRef;
using sf::core::model::EventSource;
using sf::core::model::EventSourceRef;


EventSourceRef ReadlineEventSource::_instance;
ReadlineEventSource* ReadlineEventSource::_self;

void ReadlineEventSource::handleLine(char* line) {
  if (line == nullptr) {
    ReadlineEventSource::_self->done = true;
    return;
  }

  ReadlineEventSource::_self->buffer = std::string(line);
  Static::posix()->free(line);
  ReadlineEventSource::_self->parsed = true;
}


void ReadlineEventSource::destroy() {
  ReadlineEventSource::_self = nullptr;
  ReadlineEventSource::_instance = EventSourceRef();
}

EventSourceRef ReadlineEventSource::instance(LineParser parser) {
  if (!ReadlineEventSource::_instance) {
    ReadlineEventSource* instance  = new ReadlineEventSource(parser);
    ReadlineEventSource::_instance = EventSourceRef(instance);
    ReadlineEventSource::_self = instance;
  }
  return ReadlineEventSource::_instance;
}


ReadlineEventSource::ReadlineEventSource(LineParser parser) : EventSource(
    "readline"
) {
  this->done = false;
  this->_parser  = parser;
  this->parsed   = false;
  this->input_fd = Static::posix()->dup(
      Static::posix()->fileno(::stdin)
  );
  rl_callback_handler_install("snow-fox> ", &ReadlineEventSource::handleLine);
}

bool ReadlineEventSource::hasMoreData() {
  struct pollfd set = { .fd = this->input_fd, .events = POLLIN };
  return Static::posix()->poll(&set, 1, 0) == 1;
}


ReadlineEventSource::~ReadlineEventSource() {
  rl_callback_handler_remove();
  Static::posix()->close(this->input_fd);
}

int ReadlineEventSource::getFD() {
  return this->input_fd;
}

EventRef ReadlineEventSource::parse() {
  // If readline ever return nullptr stop processing lines.
  char chr  = '\0';
  bool stop = this->done;
  this->parsed = false;

  // Read as much as possible from stdin.
  while (!stop) {
    rl_callback_read_char();
    stop = this->done || this->parsed || !this->hasMoreData();
  }

  if (this->done || this->_parser == nullptr) {
    return EventRef();
  }

  // Process the line.
  return this->_parser(this->buffer);
}
