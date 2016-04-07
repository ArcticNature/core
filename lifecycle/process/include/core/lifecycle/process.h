// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_LIFECYCLE_PROCESS_H_
#define CORE_LIFECYCLE_PROCESS_H_

namespace sf {
namespace core {
namespace lifecycle {


  //! Wrappers to trigger process::* events.
  class Process {
   public:
    static void Exit();
    static void Init();
  };

}  // namespace lifecycle
}  // namespace core
}  // namespace sf

#endif  // CORE_LIFECYCLE_PROCESS_H_
