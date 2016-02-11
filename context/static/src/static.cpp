// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/static.h"

#include <memory>

#include "core/exceptions/base.h"

using sf::core::context::Static;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::DuplicateInjection;

using sf::core::interface::Posix;
using sf::core::model::CLIParser;
using sf::core::model::Options;


// Static variables to store injected instances.
static std::shared_ptr<Options> options_ref;
static std::shared_ptr<CLIParser> parser_ref;
static std::shared_ptr<Posix> posix_ref;


// Static class.
Options* Static::options() {
  if (options_ref.get() == nullptr) {
    options_ref = std::shared_ptr<Options>(new Options());
  }
  return options_ref.get();
}

CLIParser* Static::parser() {
  if (parser_ref.get() == nullptr) {
    throw ContextUninitialised("Static cli-parser not initialised.");
  }
  return parser_ref.get();
}

Posix* Static::posix() {
  if (posix_ref.get() == nullptr) {
    throw ContextUninitialised("Static posix not initialised.");
  }
  return posix_ref.get();
}

void Static::initialise(Posix* posix) {
  if (posix_ref.get() != nullptr) {
    throw DuplicateInjection("Attempted to initialise static context twice.");
  }
  posix_ref = std::shared_ptr<Posix>(posix);
}

void Static::parser(CLIParser* parser) {
  if (parser_ref.get() != nullptr) {
    throw DuplicateInjection(
        "Attempted to initialise static cli-parser twice."
    );
  }
  parser_ref = std::shared_ptr<CLIParser>(parser);
}


void Static::destroy() {
  options_ref = std::shared_ptr<Options>();
  parser_ref  = std::shared_ptr<CLIParser>();
  posix_ref   = std::shared_ptr<Posix>();
}


// Some methods should not be used in release versions but
// are needed for tests and could be helpful for debugging.
#if DEBUG_BUILD

void Static::reset() {
  options_ref = std::shared_ptr<Options>();
  parser_ref  = std::shared_ptr<CLIParser>();
  posix_ref   = std::shared_ptr<Posix>();
}

#endif  // DEBUG_BUILD
