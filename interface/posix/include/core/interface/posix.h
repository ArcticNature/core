// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_INTERFACE_POSIX_H_
#define CORE_INTERFACE_POSIX_H_

#include <signal.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>


namespace sf {
namespace core {
namespace interface {

  //! Generic posix interface.
  /*!
   * Generic interface shared between the Posix interface and the
   * RestrictedPosix interface.
   *
   * This will allow components that are shared among both the root and
   * non-root processes (like the Forker) to use the most appropriate
   * interface depending on where are they being used.
   */
  class Posix {
   public:
    virtual ~Posix();

    // File descriptors.
    virtual int   close(int fd, bool silent = false);
    virtual FILE* freopen(const char* path, const char* mode, FILE* stream);
    virtual int   pipe(int fildes[2], int flags = 0);
    virtual ssize_t read(int fd, void *buf, size_t count, bool silent = false);
    virtual ssize_t write(int df, const void* buf, size_t count);

    // EPoll.
    virtual int epoll_control(
        int epfd, int op, int fd, struct epoll_event* event
    );
    virtual int epoll_create(int flags = 0);
    virtual int epoll_wait(
        int epfd, struct epoll_event* events,
        int maxevents, int timeout
    );

    // Memory.
    virtual void* malloc(size_t size);
    virtual void  free(void* ptr);
    virtual char* strncpy(char* dest, const char* src, size_t n);

    // Processes.
    virtual int   chdir(const char* path);
    virtual void  exit(int status);
    virtual pid_t fork();
    virtual pid_t getpid();
    virtual pid_t getppid();
    virtual int   kill(pid_t pid, int sig);
    virtual pid_t setsid();

    // Signals.
    virtual int sigaddset(sigset_t* set, int signum);
    virtual int sigemptyset(sigset_t* set);
    virtual int sigfillset(sigset_t* set);
    virtual int signalfd(int fd, const sigset_t* mask, int flags);
    virtual int sigprocmask(int how, const sigset_t* set, sigset_t*old_set);
  };

}  // namespace interface
}  // namespace core
}  // namespace sf

#endif  // CORE_INTERFACE_POSIX_H_