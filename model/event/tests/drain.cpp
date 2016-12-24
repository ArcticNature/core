// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include <exception>
#include <gtest/gtest.h>

#include "core/exceptions/base.h"
#include "core/interface/lifecycle.h"
#include "core/model/event.h"

#include "./fixtures.h"


using sf::core::exception::CorruptedData;
using sf::core::interface::Lifecycle;
using sf::core::interface::LifecycleHandlerRef;

using sf::core::lifecycle::EVENT_DRAIN_ENQUEUE;
using sf::core::lifecycle::DrainEnqueueArg;
using sf::core::lifecycle::DrainEnqueueHandler;

using sf::core::model::EventDrain;
using sf::core::model::EventDrainBuffer;
using sf::core::model::EventDrainBufferRef;
using sf::core::model::EventDrainRef;
using sf::core::model::EventRef;

using sf::testing::FailEvent;
using sf::testing::NoopEvent;
using sf::testing::TestDrain;


class TestDrainEnqueued : public DrainEnqueueHandler {
 public:
   std::string drain;
   void handle(std::string event, DrainEnqueueArg* argument) {
     this->drain = argument->drain();
   }
};


EventDrainBufferRef test_buffer(std::string bfr) {
  EventDrainBufferRef buffer(new EventDrainBuffer(bfr.length() + 1));
  memcpy(buffer->data(0), bfr.c_str(), bfr.length());
  return buffer;
};


TEST(EventDrain, Id) {
  TestDrain drain;
  ASSERT_EQ("test-drain", drain.id());
}

TEST(EventDrain, ReThrows) {
  TestDrain drain;
  std::exception_ptr ex;

  try {
    throw std::exception();
  } catch (std::exception&) {
    ex = std::current_exception();
  }

  ASSERT_THROW(drain.rescue(ex), std::exception);
}

TEST(EventDrain, Enqueue) {
  TestDrain drain;
  drain.enqueue(test_buffer("ABC"));
  drain.enqueue(test_buffer("DEF"));
  ASSERT_EQ(2, drain.length());
}

TEST(EventDrain, TriggerEnqueue) {
  EventDrainRef drain(new TestDrain());
  TestDrainEnqueued* handler = new TestDrainEnqueued();
  LifecycleHandlerRef h(handler);
  Lifecycle::reset();
  Lifecycle::on(EVENT_DRAIN_ENQUEUE, h);
  drain->enqueue(test_buffer("ABC"));
  ASSERT_EQ(drain->id(), handler->drain);
}


class TestBuffer : public EventDrainBuffer {
 public:
  TestBuffer(uint32_t size) : EventDrainBuffer(size) {
    // Noop.
  }

  char* _buffer() {
    return this->buffer;
  }

  uint32_t _consumed() {
    return this->consumed;
  }

  uint32_t _size() {
    return this->size;
  }
};


TEST(EventDrainBuffer, Create) {
  TestBuffer buffer(20);
  ASSERT_EQ(0, buffer._consumed());
  ASSERT_EQ(20, buffer._size());
  ASSERT_EQ(buffer.data(0), buffer._buffer());
}

TEST(EventDrainBuffer, Consume) {
  TestBuffer buffer(64);
  ASSERT_EQ(0, buffer._consumed());
  buffer.consume(20);
  ASSERT_EQ(20, buffer._consumed());
  buffer.consume(20);
  ASSERT_EQ(40, buffer._consumed());
  buffer.consume(40);
  ASSERT_EQ(64, buffer._consumed());
}

TEST(EventDrainBuffer, DataOffset) {
  TestBuffer buffer(64);
  char* start = buffer._buffer();
  void* data;

  data = buffer.data(0);
  ASSERT_EQ(start, data);

  data = buffer.data(33);
  ASSERT_EQ(start + 33, data);

  ASSERT_THROW(buffer.data(88), CorruptedData);
}

TEST(EventDrainBuffer, Empty) {
  TestBuffer buffer(64);
  buffer.consume(24);
  ASSERT_FALSE(buffer.empty());
  buffer.consume(40);
  ASSERT_TRUE(buffer.empty());
}

TEST(EventDrainBuffer, Full) {
  TestBuffer buffer(64);
  ASSERT_FALSE(buffer.empty());
}

TEST(EventDrainBuffer, Remaining) {
  TestBuffer buffer(64);
  char* data = buffer.remaining(nullptr);
  ASSERT_EQ(data, buffer.data(0));
}

TEST(EventDrainBuffer, RemainingWithCount) {
  TestBuffer buffer(64);
  uint32_t size = 0;
  char* data = nullptr;
  char* start = buffer._buffer();

  data = buffer.remaining(&size);
  ASSERT_EQ(data, start);
  ASSERT_EQ(64, size);

  buffer.consume(20);
  data = buffer.remaining(&size);
  ASSERT_EQ(data, start + 20);
  ASSERT_EQ(44, size);
}

TEST(EventDrainBuffer, RemainingOverflow) {
  TestBuffer buffer(64);
  uint32_t size = 0;
  buffer.consume(80);
  ASSERT_THROW(buffer.remaining(&size), CorruptedData);
}
