// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_EXCEPTIONS_EVENT_H_
#define CORE_EXCEPTIONS_EVENT_H_

#include "core/exceptions/base.h"

namespace sf {
namespace core {
namespace exception {

  //! Thrown when a non-existent drain is looked up.
  MSG_EXCEPTION(SfException, EventDrainNotFound);

}  // namespace exception
}  // namespace core
}  // namespace sf

#endif  // CORE_EXCEPTIONS_OPTIONS_H_
