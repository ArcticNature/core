// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_HOOK_H_
#define CORE_HOOK_H_

#include <functional>
#include <vector>


namespace sf {
namespace core {
namespace hook {

  //! Collection of callbacks registed to extend functionality.
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
  };

}  // namespace hook
}  // namespace core
}  // namespace sf

#include "core/hook.inc.h"

#endif  // CORE_HOOK_H_
