// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/static.h"

#include <memory>

#include "core/exceptions/base.h"

using sf::core::context::Static;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::DuplicateInjection;
using sf::core::interface::Posix;


// Static variables to store injected instances.
static bool initialised = false;
static std::shared_ptr<Posix> posix_ref;


// Static class.
Posix* Static::posix() {
  if (posix_ref.get() == nullptr) {
    throw ContextUninitialised("Static posix not initialised.");
  }
  return posix_ref.get();
}

void Static::initialise(Posix* posix) {
  if (initialised) {
    throw DuplicateInjection("Attempted to initialise static context twice.");
  }

  posix_ref   = std::shared_ptr<Posix>(posix);
  initialised = true;
}


// Some methods should not be used in release versions but
// are needed for tests and could be helpful for debugging.
#ifdef DEBUG_BUILD

void Static::reset() {
  posix_ref   = std::shared_ptr<Posix>();
  initialised = false;
}

#endif  // DEBUG_BUILD
