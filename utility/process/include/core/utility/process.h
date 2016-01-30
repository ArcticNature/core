// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_PROCESS_H_
#define CORE_UTILITY_PROCESS_H_

#include <string>

namespace sf {
namespace core {
namespace utility {

  //! Recursively resolve a symbolic link.
  /*!
   * This function returns the path pointed to by the symlink
   * give, or the parameter itself if it is not a symlink.
   *
   * Note that only symlinks that contain absolute paths
   * are supported.
   * If a symlink resolves to a relative path the last resolved
   * symlink is returned.
   *
   * @param path The path to the symbolic link to resolve.
   * @returns The path to the pointed item.
   */
  std::string resolveSymbolicLink(std::string path);

  //! Finds the directory of the current process.
  std::string processDirectory();

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_PROCESS_H_
