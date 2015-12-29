// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_REGISTRY_CLI_PARSER_H_
#define CORE_REGISTRY_CLI_PARSER_H_

#include <string>

#include "core/model/cli-parser.h"
#include "core/registry/base.h"


namespace sf {
namespace core {
namespace registry {

  //! Instantiation functions for command line parsers.
  typedef sf::core::model::CLIParser* (*make_parser)();

  //! Registry of command line parsers.
  class CLIParsers : public Registry<make_parser> {
   public:
    //! Returns the name of the parser to use.
    /*!
     * To support custom parser chosen by the user, mostly for
     * backward compatibility, an optional --parser=something
     * argument is accepted.
     *
     * This argument **must** be first and **must** have the
     * given format (--parser something is **not** valid!).
     */
    static std::string nameParser(int argc, char** argv);
  };

}  // namespace registry
}  // namespace core
}  // namespace sf

#endif  // CORE_REGISTRY_CLI_PARSER_H_
