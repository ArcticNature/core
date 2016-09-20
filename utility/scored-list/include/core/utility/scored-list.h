// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_SCORED_LIST_H_
#define CORE_UTILITY_SCORED_LIST_H_

#include <vector>

namespace sf {
namespace core {
namespace utility {

  //! List of values sorted by score.
  template<class Value>
  class ScoredList {
   protected:
    class Bucket {
     public:
      unsigned int score;
      std::vector<Value> values;

      Bucket* next;
    };

    Bucket* head;

   public:
    //! Type of ScoredList::filter predicates.
    typedef bool (*ScoredListFilter)(Value value);

    //! Value with its score.
    struct ScoredValue {
      unsigned int score;
      Value value;
    };

    ScoredList<Value>();
    ~ScoredList<Value>();

    std::vector<ScoredList<Value>::ScoredValue> filter(
        ScoredList<Value>::ScoredListFilter predicate
    );
    void insert(unsigned int score, Value value);
    std::vector<Value> pop();
  };

}  // namespace utility
}  // namespace core
}  // namespace sf

#include "core/utility/scored-list.inc.h"

#endif  // CORE_UTILITY_SCORED_LIST_H_
