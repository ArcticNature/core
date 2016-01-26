// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_MANAGER_H_
#define CORE_BIN_MANAGER_H_

#include "core/bin/async-process.h"

namespace sf {
namespace core {
namespace bin {

  //! Manager core implementataion.
  class Manager : public AsyncPorcess {
   public:
    void initialise();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_MANAGER_H_
