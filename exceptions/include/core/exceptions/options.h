// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_EXCEPTIONS_OPTIONS_H_
#define CORE_EXCEPTIONS_OPTIONS_H_

#include "core/exceptions/base.h"

namespace sf {
namespace core {
namespace exception {

  //! Thrown when a non-existent variable or oprion name is used.
  MSG_EXCEPTION(SfException, VariableNotFound);

  //! Thrown when an invalid option or value is used.
  MSG_EXCEPTION(SfException, InvalidOption);

}  // namespace exception
}  // namespace core
}  // namespace sf

#endif  // CORE_EXCEPTIONS_OPTIONS_H_
