// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_UTILITY_SCORED_LIST_INC_H_
#define CORE_UTILITY_SCORED_LIST_INC_H_

#include <vector>

namespace sf {
namespace core {
namespace utility {

  template<class Value>
  ScoredList<Value>::ScoredList() {
    this->head = nullptr;
  }

  template<class Value>
  ScoredList<Value>::~ScoredList() {
    Bucket* node = this->head;
    while (node != nullptr) {
      Bucket* next = node->next;
      delete node;
      node = next;
    }
  }

  template<class Value>
  void ScoredList<Value>::insert(unsigned int score, Value value) {
    // If there is no head create one now.
    if (this->head == nullptr) {
      this->head = new ScoredList<Value>::Bucket();
      this->head->next  = nullptr;
      this->head->score = score;
      this->head->values.push_back(value);
      return;
    }

    // If there is a head and the score is above the new one, add in front.
    if (this->head && this->head->score > score) {
      Bucket* new_head = new ScoredList<Value>::Bucket();
      new_head->next  = this->head;
      new_head->score = score;
      new_head->values.push_back(value);

      this->head->score -= score;
      this->head = new_head;
      return;
    }

    // A list exists, we need to find a bucket with
    // the same score or one with a higher one.
    Bucket* node = this->head;
    Bucket* prev = nullptr;
    unsigned int accumulator = this->head->score;

    while (node && accumulator < score) {
      prev = node;
      node = node->next;
      accumulator += node ? node->score : 0;
    }

    // A node with the right score exists.
    if (node && accumulator == score) {
      node->values.push_back(value);
      return;
    }

    // A node with higher score exists.
    if (node && accumulator > score) {
      unsigned int new_score = score - (accumulator - node->score);
      Bucket* new_node = new ScoredList<Value>::Bucket();
      new_node->next   = node;
      new_node->score  = new_score;
      new_node->values.push_back(value);

      // Update score of the moved node and redirect the previous one.
      prev->next  = new_node;
      node->score = accumulator - score;
      return;
    }

    // A new bucket is needed at the end.
    Bucket* end = new ScoredList<Value>::Bucket();
    prev->next  = end;
    end->next   = nullptr;
    end->score  = score - accumulator;
    end->values.push_back(value);
  }

  template<class Value>
  std::vector<Value> ScoredList<Value>::pop() {
    // If the head does not exists return an empty vector.
    if (!this->head) {
      return std::vector<Value>();
    }

    // If the head of the list is not 0 decrement the score.
    if (this->head && this->head->score > 0) {
      this->head->score -= 1;
      return std::vector<Value>();
    }

    // Can actually pop the top vector.
    Bucket* bucket = this->head;
    std::vector<Value> values = this->head->values;
    this->head = this->head->next;
    delete bucket;

    // When a bucket is popped, the new head (if any) needs to be
    // decremented to account for this call to `pop`.
    if (this->head) {
      this->head->score -= 1;
    }
    return values;
  }

}  // namespace utility
}  // namespace core
}  // namespace sf

#endif  // CORE_UTILITY_SCORED_LIST_INC_H_
