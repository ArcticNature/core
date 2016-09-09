// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/lua/stream.h"

#include <istream>
#include <memory>

using sf::core::utility::LuaStreamReader;

LuaStreamReader::LuaStreamReader(
    std::shared_ptr<std::istream> source, int buffer_size
) {
  this->source = source;
  this->buffer = new char[buffer_size];
  this->buffer_size = buffer_size;
}

LuaStreamReader::~LuaStreamReader() {
  delete [] this->buffer;
}

const char* LuaStreamReader::readNext(size_t* size) {
  if (this->source->eof()) {
    return nullptr;
  }

  *size = this->source->readsome(this->buffer, this->buffer_size);
  if (*size > 0) {
    return this->buffer;
  }
  return nullptr;
}

const char* LuaStreamReader::read(lua_State*, void* data, size_t* size) {
  LuaStreamReader* reader = static_cast<LuaStreamReader*>(data);
  return reader->readNext(size);
}
