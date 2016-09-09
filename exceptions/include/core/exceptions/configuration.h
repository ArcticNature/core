// Copyright 2016 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_EXCEPTIONS_CONFIGURATION_H_
#define CORE_EXCEPTIONS_CONFIGURATION_H_

#include "core/exceptions/base.h"

namespace sf {
namespace core {
namespace exception {

  //! Thrown when an invalid configuration is specified.
  MSG_EXCEPTION(SfException, InvalidConfiguration);

  //! Thrown when a required configuration is missing.
  MSG_EXCEPTION(SfException, MissingConfiguration);

}  // namespace exception
}  // namespace core
}  // namespace sf

#endif  // CORE_EXCEPTIONS_CONFIGURATION_H_
