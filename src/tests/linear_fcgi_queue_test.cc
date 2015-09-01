// fcgi_test

#include <gtest/gtest.h>

#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "linear_fcgi_queue.h"

using namespace linear::fcgi;

// linear::fcgi::Queue works with single consumer and single producer
class LinearFcgiQueueTest : public testing::Test {
protected:
  LinearFcgiQueueTest() {}
  ~LinearFcgiQueueTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(LinearFcgiQueueTest, constructor) {
  Queue<int> q;
  ASSERT_EQ(static_cast<size_t>(0), q.size());
  ASSERT_EQ(true, q.empty());
}

TEST_F(LinearFcgiQueueTest, produce_consume_in_same_thread) {
  //pthread_t pid;
  int result = -1;
  Queue<int> q;

  ASSERT_EQ(static_cast<size_t>(0), q.size());
  ASSERT_EQ(true, q.empty());
  ASSERT_EQ(false, q.consume(result));
  ASSERT_EQ(-1, result);
  q.produce(1);
  ASSERT_EQ(true, q.consume(result));
  ASSERT_EQ(1, result);
  ASSERT_EQ(static_cast<size_t>(0), q.size());
  ASSERT_EQ(true, q.empty());
}

#define NUM_OF_NODE (100)

void *producer(void *arg) {
  Queue<int>* q;

  assert(arg != NULL);
  q = reinterpret_cast<Queue<int>* >(arg);
  for (int i = 0; i < NUM_OF_NODE; i++) {
    q->produce(i);
  }
  return NULL;
}

TEST_F(LinearFcgiQueueTest, produce_in_child_thread) {
  pthread_t pid;
  int result = -1, num_of_consume = 0;
  Queue<int> q;

  ASSERT_EQ(static_cast<size_t>(0), q.size());
  ASSERT_EQ(true, q.empty());
  ASSERT_EQ(false, q.consume(result));
  ASSERT_EQ(-1, result);
  ASSERT_EQ(0, pthread_create(&pid, NULL, producer, &q));
  for (int i = 0; i < NUM_OF_NODE; i++) {
    bool ret = q.consume(result);
    usleep(10);
    if (ret) {
      ASSERT_EQ(num_of_consume, result);
      num_of_consume++;
    }
  }
  pthread_join(pid, NULL);
  ASSERT_EQ(static_cast<size_t>(NUM_OF_NODE - num_of_consume), q.size());
}

void *consumer(void *arg) {
  Queue<int>* q;
  int result = -1;
  static int num_of_consume = 0;

  assert(arg != NULL);
  q = reinterpret_cast<Queue<int>* >(arg);
  for (int i = 0; i < NUM_OF_NODE; i++) {
    usleep(1);
    bool ret = q->consume(result);
    if (ret) {
      assert(num_of_consume == result);
      num_of_consume++;
    }
  }
  return &num_of_consume;
}

TEST_F(LinearFcgiQueueTest, consume_in_child_thread) {
  pthread_t pid;
  int result = -1;
  int *num_of_consume;
  Queue<int> q;

  ASSERT_EQ(static_cast<size_t>(0), q.size());
  ASSERT_EQ(true, q.empty());
  ASSERT_EQ(false, q.consume(result));
  ASSERT_EQ(-1, result);
  ASSERT_EQ(0, pthread_create(&pid, NULL, consumer, &q));
  for (int i = 0; i < NUM_OF_NODE; i++) {
    usleep(2);
    q.produce(i);
  }
  pthread_join(pid, reinterpret_cast<void**>(&num_of_consume));
  std::cerr << "consumed: " << static_cast<int>(*num_of_consume) << std::endl;
  ASSERT_EQ(static_cast<size_t>(NUM_OF_NODE - *num_of_consume), q.size());
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
