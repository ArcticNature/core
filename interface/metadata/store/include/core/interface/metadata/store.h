// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_INTERFACE_METADATA_STORE_H_
#define CORE_INTERFACE_METADATA_STORE_H_

#include <json.hpp>
#include <poolqueue/Promise.hpp>

#include <chrono>  // NOLINT(build/c++11)
#include <memory>
#include <string>


namespace sf {
namespace core {
namespace interface {

  //! Abstract interface to Key/Value stores.
  /*!
   * Components that need to store data about their operations that is
   * expected to persist process restarts can be stored as metadata.
   *
   * This class provides a generic interface to store structured (json)
   * data with a unique identifier.
   * Concrete implementations determine how data is stored and retrived.
   *
   * Keys can also have an optional TTL which is passed to the storage
   * backend, specific implementations may have additional details for
   * the behaviour of TTLs.
   *
   * Two concrete instances of this interface are provided through the
   * Context and are for node's local and cluster'd shared metadata
   * respectively.
   * Components should access metadata though those instnaces rather
   * then creating their own metadata stores.
   */
  class MetaDataStore : public std::enable_shared_from_this<MetaDataStore> {
   public:
    virtual ~MetaDataStore() = default;

    //! Erase a key from the store.
    /*!
     * If the key does not exist nothing happens.
     *
     * If the key is removed successfully or did not exist the returned
     * promise is resolved with `nullptr`.
     *
     * If there was a problem erasing the key the returned promise is
     * rejected with the exception indicating the error.
     */
    virtual poolqueue::Promise erase(std::string key) = 0;

    //! Fetches a value for the given key.
    /*!
     * If the key is not found an `ItemNotFound` exception is used to
     * reject the returned promise.
     *
     * If a value is successfully fetched from the store,
     * a `nlohmann::json` vaule is used to resolve the returned promise.
     */
    virtual poolqueue::Promise get(std::string key) = 0;

    //! Stores/updates a value at a given key.
    /*!
     * If the value is successfully stored the returned promise
     * is resolved with `nullptr`.
     *
     * Any error is used to reject the returned promise.
     */
    virtual poolqueue::Promise set(std::string key, nlohmann::json value) = 0;

    //! Stores/updates a value at a given key with a Time To Live.
    /*!
     * Similar to `set` without a TTL, this method stores/updates a value.
     *
     * The value is valid for the given Time To Live before it is discarded.
     * See the specific implementation for details of how values are expired.
     */
    virtual poolqueue::Promise set(
        std::string key, nlohmann::json value,
        std::chrono::duration<int> ttl
    ) = 0;
  };
  typedef std::shared_ptr<MetaDataStore> MetaDataStoreRef;

}  // namespace interface
}  // namespace core
}  // namespace sf

#endif  // CORE_INTERFACE_METADATA_STORE_H_
