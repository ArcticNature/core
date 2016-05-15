// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_LIFECYCLE_CLIENT_H_
#define CORE_LIFECYCLE_CLIENT_H_

namespace sf {
namespace core {
namespace lifecycle {

  //! Interface to client lifecycle events.
  class ClientLifecycle {
   public:

    //! Subset of events dealing with the lua environment.
    class Lua {
     public:
      //! Initialise the LUA environment with client's specifics.
      static void Init();
    };
  };

}  // namespace lifecycle
}  // namespace core
}  // namespace sf

#endif  // CORE_LIFECYCLE_CLIENT_H_
