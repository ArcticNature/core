// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_EXCEPTIONS_EVENT_H_
#define CORE_EXCEPTIONS_EVENT_H_

#include "core/exceptions/base.h"

namespace sf {
namespace core {
namespace exception {

  //! Thrown when a drain is added twice.
  MSG_EXCEPTION(SfException, DuplicateEventDrain);

  //! Thrown when a source is added twice.
  MSG_EXCEPTION(SfException, DuplicateEventSource);

  //! Thrown when a non-existent drain is looked up.
  MSG_EXCEPTION(SfException, EventDrainNotFound);

  //! Thrown when a non-existent source is looked up.
  MSG_EXCEPTION(SfException, EventSourceNotFound);

  //! Thrown when a source is found but it's type is not the specified one.
  MSG_EXCEPTION(SfException, IncorrectSourceType);

}  // namespace exception
}  // namespace core
}  // namespace sf

#endif  // CORE_EXCEPTIONS_EVENT_H_
