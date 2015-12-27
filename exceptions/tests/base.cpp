#include <gtest/gtest.h>

#include "core/exceptions/base.h"

using sf::core::exception::ErrNoException;


int set_errno(int err, int ret) {
  errno = err;
  return ret;
}


TEST(Exceptions, ErrNo) {
  errno = EINVAL;
  ErrNoException exception("Test.");
  errno = 0;

  ASSERT_EQ("Test. Invalid argument (22).", std::string(exception.what()));
  ASSERT_EQ(EINVAL, exception.getCode());
}

TEST(Exceptions, CHECK_POSITIVE_ERRNO) {
  EXPECT_THROW(
    CHECK_POSITIVE_ERRNO(set_errno, "Test.", EINVAL, -1),
    ErrNoException
  );
  errno = 0;
}
