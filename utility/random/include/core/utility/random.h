// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_RANDOM_H_
#define CORE_UTILITY_RANDOM_H_

#include <string>
#include <random>

#include "core/compile-time/options.h"


namespace sf {
namespace core {
namespace utility {

  //! Generate random identifiers.
  /*!
   * Generate strings representing randomly generated ids of
   * variable length bit sequences (but only multiples of 64-bits).
   *
   * The ids are generated using the standard std::mt19937_64 C++11
   * generator seeded with a call to std::random_device.
   *
   * When building in test mode a seed can be explicitly set
   * so that the generator can be unit tested.
   */
  class IdentifierGenerator {
   public:
    IdentifierGenerator();

    #if TEST_BUILD
    //! Constructor to be used EXCLUSIVELY for tests.
    explicit IdentifierGenerator(std::random_device::result_type seed);
    #endif  // TEST_BUILD

    //! Number of 64-bit words to include in an id.
    std::string generate(uint32_t words);

   protected:
    //! The random number generator engine.
    std::mt19937_64 engine_;
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_RANDOM_H_
