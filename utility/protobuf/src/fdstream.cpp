// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/fdstream.h"

#include <cstring>

#include "core/exceptions/options.h"
#include "core/context/static.h"


using sf::core::exception::InvalidOption;
using sf::core::context::Static;
using sf::core::utility::FdStreamBuf;


FdStreamBuf::int_type FdStreamBuf::underflow() {
  if (this->gptr() < this->egptr()) {  // There is still data in the buffer.
    return FdStreamBuf::traits_type::to_int_type(*this->gptr());
  }

  // Need to read more data from fd.
  char* base  = this->input_buffer;
  char* start = base;
  size_t size = this->egptr() - this->eback();

  // Keep last N chars in the buffer.
  size = size < this->input_back ? size : this->input_back;
  if (size) {
    // Only do this after the first buffer fill.
    std::memmove(base, this->egptr() - size, size);
    start += size;
  }

  // Return EOF if read limit is set and reached.
  if (this->max_read != 0 && this->read_bytes == this->max_read) {
    this->setg(base, start, start);
    return FdStreamBuf::traits_type::eof();
  }

  // Compute number of chars that can be read into buffer.
  size_t len = this->input_size - (start - base);
  if (this->max_read != 0 && this->read_bytes + len > this->max_read) {
    len = this->max_read - this->read_bytes;
  }

  // Read data into buffer.
  ssize_t read = Static::posix()->read(this->fd, start, len, true);
  if (read <= 0) {
    this->setg(base, start, start);
    return FdStreamBuf::traits_type::eof();
  }

  // Incread read count.
  if (this->max_read != 0) {
    this->read_bytes += read;
  }

  // Return the character if possible.
  this->setg(base, start, start + read);
  return FdStreamBuf::traits_type::to_int_type(*this->gptr());
}

FdStreamBuf::pos_type FdStreamBuf::seekoff(
    FdStreamBuf::off_type off, std::ios_base::seekdir way,
    std::ios_base::openmode mode
) {
  // Only support moving on the input stream.
  if (mode != std::ios_base::in) {
    return pos_type(off_type(-1));
  }

  // Find index based on way.
  FdStreamBuf::off_type index;
  FdStreamBuf::off_type size = this->egptr() - this->eback();

  switch (way) {
    case std::ios_base::beg:
      index = 0;
      break;

    case std::ios_base::end:
      index = size;
      break;

    case std::ios_base::cur:
      index = this->gptr() - this->eback();
      break;
  }

  // Get final index and validate it.
  index += off;
  if (index < 0 || index >= size) {
    return pos_type(off_type(-1));
  }

  // Apply the offset.
  this->setg(
      this->input_buffer, this->input_buffer + index,
      this->input_buffer + size
  );
  return pos_type(off_type(index));
}


FdStreamBuf::FdStreamBuf(
    int fd, uint32_t max_read, std::size_t input_buffer,
    std::size_t input_back, std::size_t output_buffer
) {
  if (input_buffer <= 0) {
    throw InvalidOption("FdStreamBuf needs a positive buffer size.");
  }
  if (input_back < 0 && input_back >= input_buffer) {
    throw InvalidOption(
        "FdStreamBuf needs a positive backtracking size that is "
        "smaller than the buffer size."
    );
  }

  this->fd = fd;
  this->input_size = input_buffer;
  this->input_back = input_back == -1 ? this->input_size >> 2 : input_back;
  this->input_buffer = new char[this->input_size];

  // Set the input pointers to an empty situation.
  char* end = this->input_buffer + this->input_size;
  this->setg(end, end, end);

  this->read_bytes = 0;
  this->max_read = max_read;
}

FdStreamBuf::~FdStreamBuf() {
  delete [] this->input_buffer;
}
