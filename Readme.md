SnowFox core
============
This repository contains the core SnowFox components.


Component groups
----------------

  * `core.interface`:
      Interfaces are components that define how other components interact
      among each other and define the overall architecture of the system.
  * `core.model`:
      Models are components that define individual concepts fixed though
      SnowFox (daemon, clients, and everything related).
  * `core.posix`: Different posix implementations.
  * `core.protocol`: The definition files for communication portocols.
  * `core.utility`: Miscellaneous utility components.


Components
----------

### Interfaces

  * `core.interface.lifecycle`: The lifecycle system implementation.
  * `core.interface.context`: Container for all changeable settings.
  * `core.interface.posix`: Iinterface to Linux posix API.
  * `core.interface.static-context`: Container for all fixed settings.


### Models

  * `core.model.cli-parser`: Definition of a command line parser.
  * `core.model.connector`: Definition of a Spawner interface to processes.
  * `core.model.event`: Definition of an event that travels the system.
  * `core.model.event.drain`:
      Definition of an event drain, a way for events to send responses.
  * `core.model.event.manager`:
      Definition of an event manager (manages sources and fetches events).
  * `core.model.event.source`: Definition of and event source.
  * `core.model.logger`: Interface for logging events.
  * `core.model.options`: Access generic variables/options.
  * `core.model.repositoy`: The definition of a repository.
  * `core.model.service`: Service definition and state.


### Protocols

  * `core.protocol.daemon`:
    The daemon to Manager and Spawner messages, internal use.
  * `core.protocol.public`:
    The public protocol for clients to interact with the daemon.
  * `core.protocol.spawner`:
    The protocols used by the Manager and Spawner to interact with each other.

### Posix

  * `core.posix.restricted`: Posix implementation used in the Spawner.
  * `core.posix.user`: Posix implementaion used by the Manager.
