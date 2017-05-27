// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_TESTING_CLUSTER_H_
#define CORE_TESTING_CLUSTER_H_

#include "core/testing/static.h"

namespace sf {
namespace core {
namespace testing {

  class ClusterTest : public StaticContextTest {
   protected:
    ClusterTest();
    virtual ~ClusterTest();
  };

}  // namespace testing
}  // namespace core
}  // namespace sf

#endif  // CORE_TESTING_CLUSTER_H_
