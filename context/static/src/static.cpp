// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/static.h"

#include <memory>

#include "core/exceptions/base.h"

using sf::core::context::Static;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::DuplicateInjection;
using sf::core::interface::Posix;

using sf::core::model::CLIParser;
using sf::core::model::EventDrainManager;
using sf::core::model::EventDrainManagerRef;
using sf::core::model::Options;
using sf::core::model::RepositoryRef;


// Static variables to store injected instances.
static EventDrainManagerRef drains_ref;
static std::shared_ptr<Options> options_ref;
static std::shared_ptr<CLIParser> parser_ref;
static std::shared_ptr<Posix> posix_ref;
static RepositoryRef repo_ref;


// Static class.
EventDrainManager* Static::drains() {
  if (!drains_ref) {
    drains_ref = EventDrainManagerRef(new EventDrainManager());
  }
  return drains_ref.get();
}

Options* Static::options() {
  if (options_ref.get() == nullptr) {
    options_ref = std::shared_ptr<Options>(new Options());
  }
  return options_ref.get();
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

CLIParser* Static::parser() {
  if (parser_ref.get() == nullptr) {
    throw ContextUninitialised("Static cli-parser not initialised.");
  }
  return parser_ref.get();
}

void Static::parser(CLIParser* parser) {
  if (parser_ref.get() != nullptr) {
    throw DuplicateInjection(
        "Attempted to initialise static cli-parser twice."
    );
  }
  parser_ref = std::shared_ptr<CLIParser>(parser);
}

void Static::repository(RepositoryRef repo) {
  if (repo_ref.get() != nullptr) {
    throw DuplicateInjection(
        "Attempted to initialise static repository twice."
    );
  }
  repo_ref = repo;
}

RepositoryRef Static::repository() {
  if (repo_ref.get() == nullptr) {
    throw ContextUninitialised("Static repository not initialised.");
  }
  return repo_ref;
}


void Static::destroy() {
  drains_ref  = EventDrainManagerRef();
  repo_ref    = RepositoryRef();
  options_ref = std::shared_ptr<Options>();
  parser_ref  = std::shared_ptr<CLIParser>();
  posix_ref   = std::shared_ptr<Posix>();
}
