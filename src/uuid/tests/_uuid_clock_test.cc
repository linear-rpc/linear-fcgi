#include <gtest/gtest.h>

#include "_uuid_clock.h"

class _UUIDClockTest : public testing::Test {
protected:
  _UUIDClockTest() {}
  ~_UUIDClockTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(_UUIDClockTest, _uuid_get_clock) {
  uint8_t seq_hi, seq_low;
  uint8_t seq_hi_next, seq_low_next;

  ASSERT_DEATH(_uuid_get_clock(NULL, &seq_hi), "");
  ASSERT_DEATH(_uuid_get_clock(&seq_low, NULL), "");
  _uuid_get_clock(&seq_low, &seq_hi);
  _uuid_get_clock(&seq_low_next, &seq_hi_next);
  if (seq_hi_next == seq_hi) {
    ASSERT_EQ(seq_low + 1, seq_low_next);
  } else {
    ASSERT_EQ(0, seq_low_next);
    ASSERT_EQ(seq_hi + 1, seq_hi_next);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
