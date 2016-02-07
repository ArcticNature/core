// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/forker.h"

#include <unistd.h>
#include <string>

#include "core/context/static.h"
#include "core/exceptions/base.h"
#include "core/interface/posix.h"

using sf::core::context::Static;
using sf::core::exception::FileError;
using sf::core::exception::ForkFailed;

using sf::core::interface::Posix;
using sf::core::utility::Forker;


void Forker::closeNonStdFileDescriptors() {
  for (int fd=getdtablesize(); fd >= 3; --fd) {
    Static::posix()->close(fd, true);
  }
}

void Forker::dropPrivileges(std::string user, std::string group) {
  char* passwd_buffer;
  struct passwd uinfo = Static::posix()->getpwnam(
      user.c_str(), &passwd_buffer
  );
  delete [] passwd_buffer;

  if (group != "") {
    struct group ginfo = Static::posix()->getgrnam(
        group.c_str(), &passwd_buffer
    );
    delete [] passwd_buffer;
    this->dropPrivileges(uinfo.pw_uid, ginfo.gr_gid);

  } else {
    this->dropPrivileges(uinfo.pw_uid, uinfo.pw_gid);
  }
}

void Forker::dropPrivileges(uid_t user, gid_t group) {
  Static::posix()->setgroups(1, &group);
  Static::posix()->setregid(group, group);
  Static::posix()->setreuid(user, user);
  Static::posix()->setegid(group);
  Static::posix()->setgid(group);
  Static::posix()->seteuid(user);
  Static::posix()->setuid(user);
}

#define REOPEN_STD_STREAM(stream, mode)                  \
  if (stream != "" && Static::posix()->freopen(          \
        stream.c_str(), mode, ::stream                   \
    ) == nullptr) {                                      \
    throw FileError("Unable to redirect " #stream ".");  \
  }
void Forker::redirectStdFileDescriptors(
    std::string stdin, std::string stdout, std::string stderr
) {
  REOPEN_STD_STREAM(stdin,  "rb");
  REOPEN_STD_STREAM(stdout, "wb");
  REOPEN_STD_STREAM(stderr, "wb");
}


int Forker::fork() {
  pid_t pid = Static::posix()->fork();

  if (pid < 0) {  // Failed to fork.
    throw ForkFailed();

  } else if (pid == 0) {  // In child.
    this->child_pid  = Static::posix()->getpid();
    this->parent_pid = Static::posix()->getppid();
    return this->child();
  }

  // Else pid > 0 => In parent.
  this->child_pid  = pid;
  this->parent_pid = Static::posix()->getpid();
  return this->parent();
}

pid_t Forker::getChildPid() const {
  return this->child_pid;
}

pid_t Forker::getParentPid() const {
  return this->parent_pid;
}
