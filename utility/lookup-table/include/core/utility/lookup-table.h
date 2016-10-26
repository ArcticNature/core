// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_LOOKUP_TABLE_H_
#define CORE_UTILITY_LOOKUP_TABLE_H_

#include <map>
#include <string>

namespace sf {
namespace core {
namespace utility {

  //! Template class for lookup tables.
  /*!
   * This is a map indexed by std::string with some utility methods.
   */
  template<typename Value, typename DuplicateKey, typename NotFound>
  class LookupTable : public std::map<std::string, Value> {
   public:
    //! Adds an item to the table.
    /*!
     * If the key is already in the table `DuplicateKey` is thrown
     * with the key passed to the constructor.
     */
    void add(std::string key, Value value);

    //! Returns an item stored in the table.
    /*!
     * If no item with the given key is in the table `NotFound`
     * is thrown with the key passed to the constructor.
     */
    Value get(std::string key) const;

    //! Removes an item from the table.
    /*!
     * If no item with the given key is in the table `NotFound`
     * is thrown with the key passed to the constructor.
     */
    void remove(std::string id);
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#include "core/utility/lookup-table.inc.h"

#endif  // CORE_UTILITY_LOOKUP_TABLE_H_
