// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/process.h"

#include <sys/stat.h>
#include <string>

#include "core/context/static.h"

#define FALLBACK_SYMLINK_LEN 1024

using sf::core::context::Static;
using sf::core::utility::resolveSymbolicLink;


std::string sf::core::utility::resolveSymbolicLink(std::string path) {
  // Check if path is a symlink.
  struct stat info;
  Static::posix()->lstat(path.c_str(), &info);
  if (!S_ISLNK(info.st_mode)) {
    return path;
  }

  // If info.st_size is zero (which it is for /proc files) set a fixed length.
  info.st_size = FALLBACK_SYMLINK_LEN;

  // If so, resolve it.
  std::shared_ptr<char> buffer = std::shared_ptr<char>(
      new char[info.st_size]
  );
  ssize_t read = Static::posix()->readlink(
      path.c_str(), buffer.get(), info.st_size
  );

  // We only support absolute paths in links.
  // If the resolved path is not absolute, ignore it and return input.
  if (*buffer != '/') {
    return path;
  }

  // Then recursively check it.
  return resolveSymbolicLink(std::string(buffer.get(), read));
}


std::string sf::core::utility::processDirectory() {
  std::string exe   = resolveSymbolicLink("/proc/self/exe");
  std::size_t found = exe.find_last_of("/");
  return exe.substr(0, found);
}
