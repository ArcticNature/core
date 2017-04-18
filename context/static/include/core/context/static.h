// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONTEXT_STATIC_H_
#define CORE_CONTEXT_STATIC_H_

#include "core/interface/posix.h"

#include "core/promise.h"
#include "core/model/cli-parser.h"
#include "core/model/event.h"
#include "core/model/options.h"
#include "core/model/repository.h"


namespace sf {
namespace core {
namespace context {

  //! Static context for the Daemon, Manager, and Spawner processes.
  /*!
   * The static context provides access to the following:
   *
   *  * Process wide Posix implementation to use.
   */
  class Static {
   public:
    //! Keep promeses around.
    static sf::core::PromiseKeeper promises;

    //! Access drains in the system.
    static sf::core::model::EventDrainManager* drains();

    //! Cleans up all the allocated resources.
    static void destroy();

    static sf::core::model::Options*   options();
    static sf::core::interface::Posix* posix();

    //! Initialise the context at process start.
    /*!
     * @param posix The posix implementation to inject in the process.
     */
    static void initialise(sf::core::interface::Posix* posix);

    //! Gets or sets the CLIParser instance in use.
    static sf::core::model::CLIParser* parser();
    static void parser(sf::core::model::CLIParser* parser);

    //! Gets or sets the Repository instance to use.
    static sf::core::model::RepositoryRef repository();
    static void repository(sf::core::model::RepositoryRef repo);
  };

}  // namespace context
}  // namespace core
}  // namespace sf

#endif  // CORE_CONTEXT_STATIC_H_
