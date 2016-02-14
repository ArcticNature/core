SnowFox core
============
This repository contains the core SnowFox components.


Component groups
----------------

  * `core.bin`: Components for core binaries.
  * `core.context`: Components to store the different execution contexts.
  * `core.event`: Componets that form the base of the event model.
  * `core.interface`:
      Interfaces are components that define how other components interact
      among each other and define the overall architecture of the system.
  * `core.model`:
      Models are components that define individual concepts fixed though
      SnowFox (daemon, clients, and everything related).
  * `core.posix`: Different posix implementations.
  * `core.protocol`: The definition files for communication portocols.
  * `core.registry`:
      Components to store registires of different implementations of the
      interfaces.
      This is helpful to inject concrete classes without an explicit
      dependency between components.
  * `core.utility`: Miscellaneous utility components.


Components
----------


### Binaries

  * `core.bin.async-process`:
      Common classes and helpers for event based processes.
  * `core.bin.daemon`:
      Main SnowFox executable for environment setup and daemon processes
      orchestration.
  * `core.bin.manager`: Unprivileged process with the logic of the system
  * `core.bin.spawner`:
      Privileged process for restricted operations on behalf of the
      Manager process.


### Contexts
Contexts are a way to do dependency injection.
A context is a container for a set of intefraces that are used by the system
and are populated at the start with the appropriate instances of concrete
interfaces.

  * `core.context.dynamic`: Evolving and configurable context for the manager.
  * `core.context.static`: Fixed context for all processes.
  * `core.context.manager`: Extension of the static context for the Manager.
  * `core.context.spawner`: Extension of the static context for the Spawner.


### Events
Some event components are more then a model and always required:

  * `core.event.drain.fd`: EventDrain that writes to a file descriptor.
  * `core.event.source.fd`: EventSource that reads from a file descriptor.
  * `core.event.source.signal`: EventSource triggered by process signals.
  * `core.event.source.unix`:
      EventSource that waits for connections over a UNIX socket.


### Interfaces

  * `core.interface.lifecycle`: The lifecycle system implementation.
  * `core.interface.context`: Container for all changeable settings.
  * `core.interface.posix`: Interface to Linux posix API.
  * `core.interface.static-context`: Container for all fixed settings.


### Models

  * `core.model.cli-parser`: Definition of a command line parser.
  * `core.model.connector`: Definition of a Spawner interface to processes.
  * `core.model.event`:
      Base classes and features that form the event's subsystem.
      Contains event, source, drain, and manager.
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


### Registries

  * `core.registry.base`: Generic container for injectable instances.
  * `core.registry.cli-parser`: Regisrty for command line parsers.
  * `core.registry.event.managers`: Registry for event managers.
  * `core.registry.event.sources`: Registry for event sources.


### Utilities

  * `core.utility.daemoniser`:
    Base class for instances that want to daemonise themselves.
  * `core.utility.forker`: Base class for forking into multiple processes.
  * `core.utility.process`: Process related utility functions.
  * `core.utility.string`: String related utilities.


### Misc
The following core components do not actually fit in any category:

  * `core.exceptions`: Definitions of exceptions used throughout the SnowFox.
  * `core.compile-time`: A set of templates that are expanded at compile time.

### Posix

  * `core.posix.restricted`: Posix implementation used in the Spawner.
  * `core.posix.user`: Posix implementaion used by the Manager.
