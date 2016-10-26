// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/event/source/readline.h"

#include <fcntl.h>
#include <poll.h>
#include <string>

#include "readline/history.h"
#include "readline/readline.h"

#include "core/exceptions/base.h"
#include "core/compile-time/options.h"
#include "core/context/static.h"

using sf::core::context::Static;
using sf::core::event::ReadlineEventSource;
using sf::core::exception::CleanExit;

using sf::core::model::EventRef;
using sf::core::model::EventSource;
using sf::core::model::EventSourceRef;


EventSourceRef ReadlineEventSource::_instance;
ReadlineEventSource* ReadlineEventSource::_self;
std::string ReadlineEventSource::history_file;

ReadlineEventSource::ReadlineEventSource(LineParser parser) : EventSource(
    "readline"
) {
  this->done = false;
  this->_parser  = parser;
  this->parsed   = false;
  this->input_fd = Static::posix()->dup(
      Static::posix()->fileno(::stdin)
  );

  // Initialise readline and history.
  using_history();
  std::string tag = Static::options()->getString("client-id") + "> ";
  rl_callback_handler_install(tag.c_str(), &ReadlineEventSource::handleLine);
  ReadlineEventSource::readHistroyFile();
}

bool ReadlineEventSource::hasMoreData() {
  struct pollfd set = { .fd = this->input_fd, .events = POLLIN };
  return Static::posix()->poll(&set, 1, 0) == 1;
}


ReadlineEventSource::~ReadlineEventSource() {
  rl_callback_handler_remove();
  Static::posix()->close(this->input_fd);
}

int ReadlineEventSource::fd() {
  return this->input_fd;
}

EventRef ReadlineEventSource::parse() {
  // If readline ever return nullptr stop processing lines.
  bool stop = this->done;
  this->parsed = false;

  // Read as much as possible from stdin.
  while (!stop) {
    rl_callback_read_char();
    stop = this->done || this->parsed || !this->hasMoreData();
  }

  if (this->done) {
    // The user requested exit.
    throw CleanExit();
  }

  if (this->_parser == nullptr || !this->parsed) {
    return EventRef();
  }

  // Process the line.
  return this->_parser(this->buffer);
}


void ReadlineEventSource::handleLine(char* line) {
  if (line == nullptr) {
    ReadlineEventSource::_self->done = true;
    return;
  }

  // Store the line.
  std::string buffer(line);
  Static::posix()->free(line);
  ReadlineEventSource::_self->buffer = buffer;
  ReadlineEventSource::_self->parsed = true;

  // If the line was not empty, add it to the history.
  if (buffer != "") {
    ReadlineEventSource::addHistory(buffer);
  }
}

void ReadlineEventSource::readHistroyFile() {
  char*  buffer;
  uid_t  me   = Static::posix()->getuid();
  passwd info = Static::posix()->getpwuid(me, &buffer);
  std::string history_file = std::string(info.pw_dir) + "/.snowfox_history";
  ReadlineEventSource::history_file = history_file;
  delete [] buffer;

#if !TEST_BUILD
  // Make sure history file exists.
  const char* histfile = history_file.c_str();
  int tmp_fd = Static::posix()->open(
      histfile, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP
  );
  Static::posix()->close(tmp_fd);

  // Load it and move to the end.
  read_history(histfile);
  int pos = history_get_history_state()->length;
  history_set_pos(pos);
#endif
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

void ReadlineEventSource::addHistory(std::string line) {
  add_history(line.c_str());
#if !TEST_BUILD
  const char* histfile = ReadlineEventSource::history_file.c_str();
  append_history(1, histfile);
  history_truncate_file(histfile, 1000);
#endif
}

void ReadlineEventSource::clearLine() {
  rl_save_prompt();
  rl_replace_line("", 0);
  rl_redisplay();
}

void ReadlineEventSource::clearScreen() {
  rl_clear_screen(1, 0);
}

void ReadlineEventSource::showPrompt() {
  rl_restore_prompt();
  rl_redisplay();
}
