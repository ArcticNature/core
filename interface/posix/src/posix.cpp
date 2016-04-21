// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/posix.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>

#include "core/exceptions/base.h"

using sf::core::exception::ErrNoException;
using sf::core::exception::GroupNotFound;
using sf::core::exception::UserNotFound;
using sf::core::interface::Posix;


Posix::~Posix() {}


// Environment.
int Posix::clearenv() {
  return ::clearenv();
}

char* Posix::getenv(const char* name) {
  return ::getenv(name);
}

int Posix::setenv(const char* name, const char* value, int overwrite) {
  return ::setenv(name, value, overwrite);
}


// Files.
int Posix::lstat(const char* path, struct stat* buf) {
  return ::lstat(path, buf);
}

int Posix::stat(const char* path, struct stat* buf) {
  return ::stat(path, buf);
}

ssize_t Posix::readlink(const char* path, char* buf, size_t size) {
  return ::readlink(path, buf, size);
}

int Posix::unlink(const char* path) {
  CHECK_ZERO_ERRNO(::unlink, "Unable to unlink file:", path);
  return 0;
}


// File descriptors.
int Posix::close(int fd, bool silent) {
  int res = ::close(fd);
  if (!silent && res == -1) {
    throw ErrNoException("Unable to close file descriptor:");
  }
  return res;
}

int Posix::dup(int fd) {
  int res = ::dup(fd);
  if (fd == -1) {
    throw ErrNoException("Unable to dup fd:");
  }
  return res;
}

int Posix::fcntl(int fd, int cmd, int option) {
  int res = ::fcntl(fd, cmd, option);
  if (res == -1) {
    throw ErrNoException("fcntl error:");
  }
  return res;
}

FILE* Posix::freopen(const char* path, const char* mode, FILE* stream) {
  return ::freopen(path, mode, stream);
}

int Posix::open(const char* path, int flags, mode_t mode) {
  int fd = ::open(path, flags, mode);
  if (fd == -1) {
    throw ErrNoException("Unable to open file:");
  }
  return fd;
}

int Posix::pipe(int ends[2], int flags) {
  int res = ::pipe2(ends, flags | O_CLOEXEC);
  if (res == -1) {
    throw ErrNoException("Unable to create pipe:");
  }
  return res;
}

ssize_t Posix::read(int fd, void *buf, size_t count, bool silent) {
  int res = ::read(fd, buf, count);
  if (!silent && res == -1) {
    // Check if error is EAGAIN and ignore that.
    if (errno != EAGAIN) {
      throw ErrNoException("Read error:");
    }
    res = 0;
  }
  return res;
}

ssize_t Posix::write(int fd, const void* buf, size_t size) {
  int res = ::write(fd, buf, size);
  if (res == -1) {
    throw ErrNoException("Write error:");
  }
  return res;
}


// Epoll.
int Posix::epoll_control(
    int epoll_fd, int op, int fd, epoll_event* event
) {
  int result = ::epoll_ctl(epoll_fd, op, fd, event);
  if (result == -1) {
    throw ErrNoException("Epoll control failed:");
  }
  return result;
}

int Posix::epoll_create(int flags) {
  int fd = ::epoll_create1(flags);
  if (fd == -1) {
    throw ErrNoException("EPoll create failed:");
  }
  return fd;
}

int Posix::epoll_wait(
    int epfd, epoll_event* events, int max, int timeout
) {
  int result = ::epoll_wait(epfd, events, max, timeout);
  if (result == -1) {
    throw ErrNoException("Epoll wait failed:");
  }
  return result;
}


// Memory.
void* Posix::malloc(size_t size) {
  return ::malloc(size);
}

void Posix::free(void* block) {
  ::free(block);
}

char* Posix::strncpy(char* dest, const char* src, size_t len) {
  return ::strncpy(dest, src, len);
}


// Others.
unsigned int Posix::sleep(unsigned int seconds) {
  return ::sleep(seconds);
}


// Process.
void Posix::exit(int status) {
  ::exit(status);
}

int Posix::execvp(const char* file, char* const argv[]) {
  return ::execvp(file, argv);
}

pid_t Posix::fork() {
  return ::fork();
}

int Posix::kill(pid_t pid, int sig) {
  return ::kill(pid, sig);
}

pid_t Posix::waitpid(pid_t pid, int* status, int options) {
  return ::waitpid(pid, status, options);
}

int Posix::chdir(const char* path) {
  return ::chdir(path);
}

char* Posix::getcwd(char* buf, size_t size) {
  return ::getcwd(buf, size);
}

pid_t Posix::getpid() {
  return ::getpid();
}

pid_t Posix::getppid() {
  return ::getppid();
}

pid_t Posix::setsid() {
  return ::setsid();
}


// Signals.
int Posix::sigaddset(sigset_t* set, int signal) {
  int res = ::sigaddset(set, signal);
  if (res == -1) {
    throw ErrNoException("Unable to create sigaddset:");
  }
  return res;
}

int Posix::sigemptyset(sigset_t* set) {
  int res = ::sigemptyset(set);
  if (res == -1) {
    throw ErrNoException("Unable to create sigemptyset:");
  }
  return res;
}

int Posix::sigfillset(sigset_t* set) {
  int res = ::sigfillset(set);
  if (res == -1) {
    throw ErrNoException("Unable to create sigfillset:");
  }
  return res;
}

int Posix::signalfd(int fd, const sigset_t *mask, int flags) {
  int res = ::signalfd(fd, mask, flags);
  if (res == -1) {
    throw ErrNoException("Unable to create signalfd:");
  }
  return res;
}

int Posix::sigprocmask(
    int how, const sigset_t* set, sigset_t* oldset
) {
  int res = ::sigprocmask(how, set, oldset);
  if (res == -1) {
    throw ErrNoException("Unable to mask signals:");
  }
  return res;
}


// Socket.
int Posix::accept(
    int sockfd, struct sockaddr* addr, socklen_t* addrlen, int flags
) {
  int res = ::accept4(sockfd, addr, addrlen, flags);
  if (res == -1) {
    throw ErrNoException("Unable to accept socket:");
  }
  return res;
}

int Posix::bind(
    int sockfd, const struct sockaddr* addr, socklen_t addrlen
) {
  CHECK_ZERO_ERRNO(::bind, "Unable to bind socket:", sockfd, addr, addrlen);
  return 0;
}

int Posix::connect(
    int sockfd, const struct sockaddr* addr, socklen_t addrlen
) {
  CHECK_ZERO_ERRNO(
      ::connect, "Unable to connect to socket:",
      sockfd, addr, addrlen
  );
  return 0;
}

int Posix::listen(int sockfd, int backlog) {
  CHECK_ZERO_ERRNO(::listen, "Unable to listen on socket:", sockfd, backlog);
  return 0;
}

ssize_t Posix::recv(int sockfd, void* buf, size_t len, int flags) {
  ssize_t size = ::recv(sockfd, buf, len, flags);
  if (size == -1) {
    throw ErrNoException("Unable to recv:");
  }
  return size;
}

int Posix::socket(int domain, int type, int protocol) {
  int fd = ::socket(domain, type, protocol);
  if (fd == -1) {
    throw ErrNoException("Unable to open socket:");
  }
  return fd;
}

int Posix::shutdown(int sockfd, int how) {
  CHECK_ZERO_ERRNO(::shutdown, "Unable to shutdown socket:", sockfd, how);
}


// Users.
struct group Posix::getgrnam(const char* name, char** buf) {
  size_t bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  if (bufsize == -1) {  /* Value was indeterminate */
    bufsize = 16384;    /* Should be more than enough */
  }

  char* buffer = new char[bufsize];
  *buf = buffer;

  struct group  ginfo;
  struct group* result;
  ::getgrnam_r(name, &ginfo, buffer, bufsize, &result);

  if (result == nullptr) {
    throw GroupNotFound("Unable to find group '" + std::string(name) + "'.");
  }
  return ginfo;
}

struct passwd Posix::getpwnam(const char* name, char** buf) {
  size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1) {  /* Value was indeterminate */
    bufsize = 16384;    /* Should be more than enough */
  }

  char* buffer = new char[bufsize];
  *buf = buffer;

  struct passwd  uinfo;
  struct passwd* result;
  getpwnam_r(name, &uinfo, buffer, bufsize, &result);

  if (result == nullptr) {
    delete [] buffer;
    throw UserNotFound("Unable to find user '" + std::string(name) + "'.");
  }

  return uinfo;
}

int Posix::setgroups(int size, gid_t list[]) {
  CHECK_POSITIVE_ERRNO(
    ::setgroups, "Unable to drop secondary groups.", 1, list
  );
}

int Posix::setegid(gid_t egid) {
  CHECK_POSITIVE_ERRNO(::setegid, "Unable to drop effective group.", egid);
}

int Posix::seteuid(uid_t euid) {
  CHECK_POSITIVE_ERRNO(::seteuid, "Unable to drop effective user.", euid);
}

int Posix::setgid(gid_t gid) {
  CHECK_POSITIVE_ERRNO(::setgid,  "Unable to drop group.", gid);
}

int Posix::setuid(uid_t uid) {
  CHECK_POSITIVE_ERRNO(::setuid,  "Unable to drop user.", uid);
}

int Posix::setregid(gid_t rgid, gid_t egid) {
  CHECK_POSITIVE_ERRNO(::setregid, "Unable to drop saved group.", egid, egid);
}

int Posix::setreuid(uid_t ruid, uid_t euid) {
  CHECK_POSITIVE_ERRNO(::setreuid, "Unable to drop saved user.", euid, euid);
}
