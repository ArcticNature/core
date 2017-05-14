// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_HOOK_H_
#define CORE_HOOK_H_

#include <exception>
#include <functional>
#include <vector>

#include "core/compile-time/options.h"


namespace sf {
namespace core {
namespace hook {

  //! Represents an event, signal, or extention point.
  template<typename ...HookArgs>
  class Hook {
   protected:
    typedef std::function<void(HookArgs...)> CBType;
    typedef typename std::vector<CBType>::iterator CBIter;

    bool rethrow_;
    std::vector<CBType> callbacks_;

   public:
    explicit Hook(bool rethrow = false);

    //! Adds a new callback to be called on trigger.
    void attach(CBType callback);

    //! Removes a registered callback.
    void detach(CBType callback);

    //! Invokes the callbacks forwarding all passed arguments.
    void trigger(HookArgs...);

#if TEST_BUILD
    //! Removes all registered callbacks.
    void clear();
#endif  // TEST_BUILD
  };

  //! Hook used to allow extensible handling of exceptions.
  typedef Hook<std::exception_ptr> ErrorHook;

  //! Hook used to react to something happening in the system.
  typedef Hook<> SignalHook;

}  // namespace hook
}  // namespace core
}  // namespace sf

#include "core/hook.inc.h"

#endif  // CORE_HOOK_H_
