#include <gtest/gtest.h>

#include "_uuid_time.h"

class _UUIDTimeTest : public testing::Test {
protected:
  _UUIDTimeTest() {}
  ~_UUIDTimeTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(_UUIDTimeTest, _uuid_get_time) {
  uint32_t time_low;
  uint16_t time_mid, time_hi;

  ASSERT_DEATH(_uuid_get_time(NULL, &time_mid, &time_hi), "");
  ASSERT_DEATH(_uuid_get_time(&time_low, NULL, &time_hi), "");
  ASSERT_DEATH(_uuid_get_time(&time_low, &time_mid, NULL), "");
  _uuid_get_time(&time_low, &time_mid, &time_hi);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
