// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_STRING_H_
#define CORE_UTILITY_STRING_H_

#include <string>
#include <vector>

namespace sf {
namespace core {
namespace utility {
namespace string {

  //! Returns the current date and time in YYYY-MM-DD HH:MM:SS format.
  std::string currentTimestamp();

  //! Finds the first, non-escaped occurrence of a string in another.
  std::string::size_type findFirsNotEscaped(
      std::string what, std::string where,
      std::string::size_type start = 0, char escape = '\\'
  );

  //! Joins a string on the given delimitor.
  std::string join(
      const std::vector<std::string> source, char delimit,
      std::vector<std::string>::size_type size
  );

  //! Splits a string on the given delimitor.
  std::vector<std::string> split(
      const std::string source, char delimit, int count = -1
  );

  //! Converts a number into a string.
  std::string toString(int value);

}  // namespace string
}  // namespace utility
}  // namespace core
}  // namespace sf


#endif  // CORE_UTILITY_STRING_H_
