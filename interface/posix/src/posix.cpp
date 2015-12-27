// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/posix.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/signalfd.h>

#include "core/exceptions/base.h"

using sf::core::exception::ErrNoException;
using sf::core::interface::Posix;


Posix::~Posix() {}

int Posix::close(int fd, bool silent) {
  int res = ::close(fd);
  if (!silent && res == -1) {
    throw ErrNoException("Unable to close file descriptor:");
  }
  return res;
}

FILE* Posix::freopen(const char* path, const char* mode, FILE* stream) {
  return ::freopen(path, mode, stream);
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


int Posix::epoll_control(
    int epoll_fd, int op, int fd, epoll_event* event
) {
  return epoll_ctl(epoll_fd, op, fd, event);
}

int Posix::epoll_create(int flags) {
  int fd = epoll_create1(flags);
  if (fd == -1) {
    throw ErrNoException("EPoll failed:");
  }
  return fd;
}

int Posix::epoll_wait(
    int epfd, epoll_event* events, int max, int timeout
) {
  return ::epoll_wait(epfd, events, max, timeout);
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


// Process.
int Posix::chdir(const char* path) {
  return ::chdir(path);
}

void Posix::exit(int status) {
  ::exit(status);
}

pid_t Posix::fork() {
  return ::fork();
}

pid_t Posix::getpid() {
  return ::getpid();
}

pid_t Posix::getppid() {
  return ::getppid();
}

int Posix::kill(pid_t pid, int sig) {
  return ::kill(pid, sig);
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
