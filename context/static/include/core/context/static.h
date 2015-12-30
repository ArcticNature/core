// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONTEXT_STATIC_H_
#define CORE_CONTEXT_STATIC_H_

#include "core/compile-time/options.h"
#include "core/interface/posix.h"
#include "core/model/cli-parser.h"


namespace sf {
namespace core {
namespace context {

  //! Static context for the Daemon, Manager, and Spawner processes.
  /**
   * The static context provides access to the following:
   *
   *  * Process wide Posix implementation to use.
   */
  class Static {
   public:
    static sf::core::model::CLIParser* parser();
    static sf::core::interface::Posix* posix();

    //! Initialise the context at process start.
    /*
     * @param posix The posix implementation to inject in the process.
     */
    static void initialise(sf::core::interface::Posix* posix);

    //! Set the CLIParser instance in use.
    static void parser(sf::core::model::CLIParser* parser);

#ifdef DEBUG_BUILD
    static void reset();
#endif  // DEBUG_BUILD
  };

}  // namespace context
}  // namespace core
}  // namespace sf

#endif  // CORE_CONTEXT_STATIC_H_
