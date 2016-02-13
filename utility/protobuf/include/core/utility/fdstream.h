// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_FDSTREAM_H_
#define CORE_UTILITY_FDSTREAM_H_

#include <streambuf>

#include "core/compile-time/options.h"


namespace sf {
namespace core {
namespace utility {

  //! IO stream buffer for unix file descriptors.
  /*!
   * Wrapper class for file descriptor opened and managed with
   * unix file functions.
   * Mainly intended for use to stream sockets for Protocol Buffers.
   * 
   * Instances support both reading and writing to/from the file descriptor.
   * The size of the input buffer and the output buffer can be specified
   * at creation time and may be different.
   * 
   * Reading and writing is done through a Posix interface so the
   * specific implementation of the methods can be changed if needed.
   * 
   * Supported operations:
   *   * Buffering of data (both read and write).
   *   * Configurable buffer and seek sizes.
   *   * Seek read position (but not write).
   */
  class FdStreamBuf : public std::streambuf {
   protected:
    int fd;  //!< File descriptor wrapped by the stream.

    char* input_buffer;      //!< Pointer to the input buffer.
    std::size_t input_back;  //!< How many chars are kept to go back.
    std::size_t input_size;  //!< Size of the input buffer.

    uint32_t max_read;    //!< Maximum number of bytes that can be read.
    uint32_t read_bytes;  //!< Number of bytes read so far.

    virtual int_type underflow();
    virtual pos_type seekoff(
        off_type off, std::ios_base::seekdir way,
        std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out
    );

   public:
    //! Constant to disable read limits on the stream.
    static const uint32_t READ_ALL = 0;

    FdStreamBuf(
        int fd, uint32_t max_read = FdStreamBuf::READ_ALL,
        std::size_t input_buffer = DEFAULT_BUFFER_SIZE,
        std::size_t input_back = -1,
        std::size_t output_buffer = DEFAULT_BUFFER_SIZE
    );
    ~FdStreamBuf();
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_FDSTREAM_H_
