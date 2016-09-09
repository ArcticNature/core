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
  * `core.lifecycle`: Definitions and helpers for Lifecycle events.
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
  * `core.bin.client`: Command line interface to the server.
  * `core.bin.daemon`:
      Main SnowFox executable for environment setup and daemon processes
      orchestration.
  * `core.bin.manager`: Unprivileged process with the logic of the system
  * `core.bin.spawner`:
      Privileged process for restricted operations on behalf of the
      Manager process.


### Cluster

  * `core.cluster.node`: Represents and manipulates a cluster node.


### Contexts
Contexts are a way to do dependency injection.
A context is a container for a set of intefraces that are used by the system
and are populated at the start with the appropriate instances of concrete
interfaces.

  * `core.context.client`: Stores a client specific runtime context.
  * `core.context.daemon`: Stores a daemon specific runtime context.
  * `core.context.dynamic`: Evolving and configurable context for the manager.
  * `core.context.event`: Framework for event correlation runtime contexts.
  * `core.context.static`: Fixed context for all processes.


### Events
Some event components are more then a model and always required:

  * `core.event.client.unix`: EventSource that connects to a UNIX socket.
  * `core.event.drain.fd`: EventDrain that writes to a file descriptor.
  * `core.event.drain.null`: EventDrain that writed to `/dev/null`.
  * `core.event.source.connected`: Socket connection-aware EventSource.
  * `core.event.source.fd`: EventSource that reads from a file descriptor.
  * `core.event.source.manaual`: EventSource that emits manually enqueued events.
  * `core.event.source.readline`:
      EventSource that reads stdin through GNU readline.
  * `core.event.source.scheduled`:
      EventSource that emits events sheduled by other components.
  * `core.event.source.signal`: EventSource triggered by process signals.
  * `core.event.source.unix`:
      EventSource that waits for connections over a UNIX socket.


### Interfaces

  * `core.interface.config`: Configuration loaders for nodes, services, etc ...
  * `core.interface.lifecycle`: The lifecycle system implementation.
  * `core.interface.posix`: Interface to Linux posix API.


### Lifecycles

  * `core.lifecycle.client`: Client specific lifecycle steps.
  * `core.lifecycle.event`: Event lifecycle helpers and steps.
  * `core.lifecycle.process`: Unix process lifecycle events.


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

  * `core.protocol.manager_spawner`: The Manager to Spawner messages.
  * `core.protocol.daemon_manager`: The Daemon to Manager messages.
  * `core.protocol.daemon_spawner`: The Daemon to Spawner messages.
  * `core.protocol.public`:
    The public protocol for clients to interact with the daemon.
  * `core.protocol.test`: Protocol used for tests.


### Registries

  * `core.registry.base`: Generic container for injectable instances.
  * `core.registry.cli-parser`: Regisrty for command line parsers.
  * `core.registry.event.handler`: Registry for protobuf to EventRef factories.
  * `core.registry.event.managers`: Registry for event managers.
  * `core.registry.event.sources`: Registry for event sources.
  * `core.registry.repositories`: Registry of supported configuration repos.


### Utilities

  * `core.utility.daemoniser`:
    Base class for instances that want to daemonise themselves.
  * `core.utility.forker`: Base class for forking into multiple processes.
  * `core.utility.lua`: Wrappert classes for LUA.
  * `core.utility.process`: Process related utility functions.
  * `core.utility.protobuf`: Utilitiy functions to send/receive protocol buffers.
  * `core.utility.scored-list`: Linked lists of elements sorted by score.
  * `core.utility.status`: Store and manipulate status information.
  * `core.utility.string`: String related utilities.


### Misc
The following core components do not actually fit in any category:

  * `core.exceptions`: Definitions of exceptions used throughout the SnowFox.
  * `core.compile-time`: A set of templates that are expanded at compile time.

### Posix

  * `core.posix.restricted`: Posix implementation used in the Spawner.
  * `core.posix.user`: Posix implementaion used by the Manager.
