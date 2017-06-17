// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include <gtest/gtest.h>
#include <poolqueue/Promise.hpp>

#include "core/testing/promise.h"


using poolqueue::Promise;


void EXPECT_PROMISE_NO_THROW(Promise promise) {
  promise.except([](const std::exception_ptr& exception) {
    try {
      std::rethrow_exception(exception);
    } catch (std::exception& ex) {
      ADD_FAILURE() << "Promise throws -- " << ex.what();
    }
    return nullptr;
  });
}

void EXPECT_PROMISE_RESOLVED(Promise promise) {
  EXPECT_TRUE(promise.settled());
  EXPECT_PROMISE_NO_THROW(promise);
}
