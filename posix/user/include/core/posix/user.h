// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_POSIX_USER_H_
#define CORE_POSIX_USER_H_

#include <dirent.h>
#include <netdb.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <string>

#include "core/interface/posix.h"


namespace sf {
namespace core {
namespace posix {

  //! Type of a signal handler.
  typedef void (*signal_handler_t) (int);

  class User : public sf::core::interface::Posix {
   public:
    virtual ~User();

    // Directories.
    virtual DIR* opendir(const char* name);
    virtual struct dirent* readdir(DIR* dirp);
    virtual int closedir(DIR *dirp);

    // EventFD.
    virtual int eventfd(unsigned int initval, int flags);

    // Files.
    virtual bool exists(std::string path);
    virtual void lstat(const char *path, struct stat *buf);
    virtual int  mkdir(const char *pathname, mode_t mode);
    virtual ssize_t readlink(const char *path, char *buf, size_t bufsiz);
    virtual int unlink(std::string filename);

    // Socket.
    virtual int  accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    virtual int  bind(int fd, const struct sockaddr* addr, socklen_t len);
    virtual void freeaddrinfo(struct addrinfo* res);
    virtual int  listen(int fd, int backlog);
    virtual ssize_t send(int sockfd, const void* buf, size_t len, int flags);
    virtual int resolve(
        std::string address, int port, int type, int flags,
        struct addrinfo** resolved
    );
    virtual int setsockopt(int fd, int level, int option_name, bool enabled);
    virtual int socket(int domain, int type, int protocol);

    // TimerFD.
    virtual int timerfd_create(int clockid, int flags);
    virtual int timerfd_settime(
        int fd, int flags, const struct itimerspec *new_value,
        struct itimerspec *old_value
    );
  };

}  // namespace posix
}  // namespace core
}  // namespace sf

#endif  // CORE_POSIX_USER_H_
