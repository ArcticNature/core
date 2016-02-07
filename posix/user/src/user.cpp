// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/posix/user.h"

#include <signal.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <sys/timerfd.h>

#include <unistd.h>
#include <sys/socket.h>

#include <string>

#include "core/exceptions/base.h"
#include "core/utility/string.h"


using sf::core::exception::ErrNoException;
using sf::core::exception::SocketException;
using sf::core::posix::User;
using sf::core::utility::string::toString;


User::~User() {}

// Directories.
DIR* User::opendir(const char* name) {
  return ::opendir(name);
}

struct dirent* User::readdir(DIR* dirp) {
  return ::readdir(dirp);
}

int User::closedir(DIR *dirp) {
  return ::closedir(dirp);
}

// EventFD.
int User::eventfd(unsigned int initval, int flags) {
  return ::eventfd(initval, flags);
}

// Files.
bool User::exists(std::string path) {
  struct stat buffer;
  return ::stat(path.c_str(), &buffer) == 0;
}

int User::lstat(const char* path, struct stat* buf) {
  CHECK_ZERO_ERRNO(::lstat, "Unable to stat file:", path, buf);
}

int User::mkdir(const char* pathname, mode_t mode) {
  CHECK_ZERO_ERRNO(::mkdir, "Unable to create directory: ", pathname, mode);
}

ssize_t User::readlink(const char *path, char *buf, size_t bufsiz) {
  int res = ::readlink(path, buf, bufsiz);
  if (res == -1) {
    throw ErrNoException("Unable to read link: ");
  }
  return res;
}

int User::unlink(std::string filename) {
  return ::unlink(filename.c_str());
}

// Socket.
int User::accept(int fd, struct sockaddr* addr, socklen_t* len) {
  return ::accept(fd, addr, len);
}

int User::bind(int fd, const struct sockaddr* addr, socklen_t len) {
  CHECK_ZERO_ERRNO(::bind, "Unable to bind socket:", fd, addr, len);
  return 0;
}

void User::freeaddrinfo(struct addrinfo* info) {
  ::freeaddrinfo(info);
}

int User::listen(int fd, int backlog) {
  CHECK_ZERO_ERRNO(::listen, "Unable to listen on socket:", fd, backlog);
  return 0;
}

int User::resolve(
    std::string address, int port, int type, int flags,
    struct addrinfo** resolved
) {
  struct addrinfo hints    = {0};
  std::string     str_port = toString(port);

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = type;
  hints.ai_flags    = flags;

  int res = ::getaddrinfo(address.c_str(), str_port.c_str(), &hints, resolved);
  if (res != 0) {
    throw SocketException(res, gai_strerror(res));
  }
  return res;
}

ssize_t User::send(int fd, const void* buffer, size_t length, int flags) {
  return ::send(fd, buffer, length, flags);
}

int User::setsockopt(int fd, int level, int option_name, bool enabled) {
  int value = enabled ? 1 : 0;
  return ::setsockopt(fd, level, option_name, &value, sizeof value);
}

int User::socket(int domain, int type, int protocol) {
  int fd = ::socket(domain, type, protocol);
  if (fd < 0) {
    throw ErrNoException("Unable to open socket:");
  }
  return fd;
}

// TimerFD.
int User::timerfd_create(int clockid, int flags) {
  return ::timerfd_create(clockid, flags);
}

int User::timerfd_settime(
    int fd, int flags, const struct itimerspec *new_value,
    struct itimerspec *old_value
) {
  return ::timerfd_settime(fd, flags, new_value, old_value);
}
