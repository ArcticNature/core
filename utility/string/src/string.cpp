// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/string.h"

#include <time.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>


std::string sf::core::utility::string::currentTimestamp() {
  time_t    current_time = time(nullptr);
  char      formatted[42];
  struct tm structured_time;
  localtime_r(&current_time, &structured_time);
  strftime(formatted, sizeof(formatted), "%Y-%m-%d %H:%M:%S", &structured_time);
  return formatted;
}

std::string::size_type sf::core::utility::string::findFirsNotEscaped(
    std::string what, std::string where,
    std::string::size_type start, char escape
) {
  std::string::size_type matched = 0;
  std::string::size_type pos     = 0;

  for (std::string::size_type idx = start; idx < where.length(); idx++) {
    char current = where[idx];

    if (current == escape) {
      // Reset match, if any, and skip next character.
      matched = 0;
      idx++;

    } else if (current == what[matched]) {
      if (matched == 0) {
        pos = idx;
      }
      matched++;

      if (matched >= what.length()) {
        return pos;
      }

    } else {
      // Partial match did not succeed.
      matched = 0;
      pos     = 0;
    }
  }

  // Could not find any match.
  return -1;
}

std::string sf::core::utility::string::join(
    const std::vector<std::string> source, char delimit,
    std::vector<std::string>::size_type size
) {
  if (source.size() < size) {
    size = source.size();
  }
  if (size == 0) {
    return "";
  }
  if (size == 1) {
    return source[0];
  }

  std::stringstream buffer;
  buffer << source[0];
  for (std::vector<std::string>::size_type idx = 1; idx < size; idx++) {
    buffer << delimit << source[idx];
  }
  return buffer.str();
}

std::vector<std::string> sf::core::utility::string::split(
    const std::string source, char delimit, int count
) {
  std::vector<std::string> result;
  std::string::size_type   start = 0;
  std::string::size_type   end   = source.find_first_of(delimit);

  while (end != std::string::npos && (count == -1 || count > 1)) {
    result.push_back(source.substr(start, end - start));
    start  = end + 1;
    end    = source.find_first_of(delimit, start);
    count -= count == -1 ? 0 : 1;
  }
  result.push_back(source.substr(start, source.length() - start));
  return result;
}

std::string sf::core::utility::string::toString(int value) {
  std::stringstream stream;
  stream << value;
  return stream.str();
}
