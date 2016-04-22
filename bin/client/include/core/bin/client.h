// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_BIN_CLIENT_H_
#define CORE_BIN_CLIENT_H_

#include <string>

#include "core/bin/async-process.h"


namespace sf {
namespace core {
namespace bin {

  //! Command line client implementataion.
  class Client : public AsyncPorcess {
   protected:
    void connectToServer();
    void maskSignals();

   public:
    void initialise();
  };

}  // namespace bin
}  // namespace core
}  // namespace sf

#endif  // CORE_BIN_CLIENT_H_
