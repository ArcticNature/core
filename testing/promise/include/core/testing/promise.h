// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_TESTING_PROMISE_H_
#define CORE_TESTING_PROMISE_H_

#include <poolqueue/Promise.hpp>

//! Asserts a promise does not throw an exception.
void EXPECT_PROMISE_NO_THROW(poolqueue::Promise promise);

//! Asserts a promise has settled without error.
void EXPECT_PROMISE_RESOLVED(poolqueue::Promise promise);

#endif  // CORE_TESTING_PROMISE_H_
