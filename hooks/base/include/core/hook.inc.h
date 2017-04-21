// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_HOOK_INC_H_
#define CORE_HOOK_INC_H_

#include <vector>


namespace sf {
namespace core {
namespace hook {

  template<typename ...HookArgs>
  Hook<HookArgs...>::Hook(bool rethrow) {
    this->rethrow_ = rethrow;
  }

  template<typename ...HookArgs>
  void Hook<HookArgs...>::attach(Hook<HookArgs...>::CBType callback) {
    this->callbacks_.push_back(callback);
  }

  template<typename ...HookArgs>
  void Hook<HookArgs...>::detach(Hook<HookArgs...>::CBType callback) {
    typename std::vector<Hook<HookArgs...>::CBIter> to_delete;
    for (auto it = this->callbacks_.begin();
          it != this->callbacks_.end(); it++) {
      if (it->target_type() == callback.target_type()) {
        to_delete.push_back(it);
      }
    }

    for (auto it : to_delete) {
      this->callbacks_.erase(it);
    }
  }

  template<typename ...HookArgs>
  void Hook<HookArgs...>::trigger(HookArgs... args) {
    for (auto cb : this->callbacks_) {
      try {
        cb(args...);
      } catch (std::exception& ex) {
        if (this->rethrow_) {
          throw;
        }
      }
    }
  }

}  // namespace hook
}  // namespace core
}  // namespace sf

#endif  // CORE_HOOK_INC_H_
