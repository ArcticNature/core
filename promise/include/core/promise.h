// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_PROMISE_H_
#define CORE_PROMISE_H_

#include <poolqueue/Promise.hpp>

#include <map>
#include <string>
#include <vector>


namespace sf {
namespace core {

  //! Keep track of promese that are active across the system.
  /*!
   * Each promise is identified by a unique ID.
   *
   * The keeper can hold promeses that will be resolved by other parts of
   * the system (such as external systems or other nodes).
   * This allows decoupling of requestors and responders.
   * 
   * Promeses also get an optional timeout (TTL in ticks).
   * If a promise is kept in the system for longer then its timeout
   * the keeper will reject it with a TimeoutError.
   */
  class PromiseKeeper {
   protected:
    //! Store promises with needed metadata.
    struct PMetaData {
     public:
      bool expire;
      uint32_t ttl;
      poolqueue::Promise promise;
    };

    //! Stores promeses by ID.
    std::map<std::string, PMetaData> promises_;

   public:
    //! Pops all promeses and rejects them.
    void clear();

    //! Adds a promise to the keeper.
    void keep(std::string id, poolqueue::Promise promise, unsigned int ttl = 0);

    //! Removes and returns a promise to the keeper.
    poolqueue::Promise pop(std::string id);

    //! Decrease promeses TTL and rejects any promise that timed out.
    void tick();
  };

}  // namespace core
}  // namespace sf


#endif  // CORE_PROMISE_H_
