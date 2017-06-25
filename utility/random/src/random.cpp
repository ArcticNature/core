// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "core/utility/random.h"

#include <sstream>
#include <string>

#include "core/exceptions/base.h"


using sf::core::exception::InvalidArgument;
using sf::core::utility::IdentifierGenerator;


IdentifierGenerator::IdentifierGenerator()
  : engine_(std::random_device()()) {
  // NOOP.
}

#if TEST_BUILD
IdentifierGenerator::IdentifierGenerator(
    std::random_device::result_type seed
) : engine_(seed) {
  // NOOP.
}
#endif  // TEST_BUILD


std::string IdentifierGenerator::generate(uint32_t words) {
  if (words == 0) {
    throw InvalidArgument("Must generate at least one word");
  }

  // Generate random id.
  std::stringstream buffer;
  buffer << std::hex;
  for (uint32_t idx = 0; idx < words; idx++) {
    buffer << this->engine_();
  }
  return buffer.str();
}
