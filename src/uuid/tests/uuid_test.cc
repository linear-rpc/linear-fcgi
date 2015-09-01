#include <stdio.h>
#include <string.h>
#include <gtest/gtest.h>

#include "uuid.h"

using namespace std;

class UUIDTest : public testing::Test {
protected:
  UUIDTest() {}
  ~UUIDTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(UUIDTest, uuid_clear) {
  uuid_t exp = {0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0};
  uuid_t uu = {1, 2, 3, 4, 5, 6, 7, 8,
               9, 10, 11, 12, 13, 14, 15, 16};

  uuid_clear(uu);
  ASSERT_EQ(0, memcmp(exp, uu, sizeof(uuid_t)));
}

TEST_F(UUIDTest, _uuid_compare) {
  uuid_t uu1 = {0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0};
  uuid_t uu2 = {0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0};

  ASSERT_EQ(0, uuid_compare(uu1, uu2));
  uu1[15] = 0;
  uu2[15] = 1;
  ASSERT_GT(0, uuid_compare(uu1, uu2)); // -1: uu1 < uu2
  uu1[15] = 1;
  uu2[15] = 0;
  ASSERT_LT(0, uuid_compare(uu1, uu2)); //  1: uu1 > uu2
}

TEST_F(UUIDTest, uuid_copy) {
  uuid_t src = {1, 2, 3, 4, 5, 6, 7, 8,
                9, 10, 11, 12, 13, 14, 15, 16};
  uuid_t dst = {0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0};
  uuid_t exp = {1, 2, 3, 4, 5, 6, 7, 8,
                9, 10, 11, 12, 13, 14, 15, 16};

  uuid_copy(dst, src);
  ASSERT_EQ(0, memcmp(exp, dst, sizeof(uuid_t)));
}

TEST_F(UUIDTest, uuid_is_null) {
  uuid_t notnull = {0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 1};
  uuid_t null = {0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, 0};
  ASSERT_EQ(0, uuid_is_null(notnull));
  ASSERT_EQ(1, uuid_is_null(null));
}

TEST_F(UUIDTest, uuid_generate) {
  uuid_t out;

  uuid_generate(out);
  ASSERT_EQ(0, uuid_is_null(out));
}

TEST_F(UUIDTest, uuid_parse) {
  uuid_t exp = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                0x01, 0x23, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34};
  uuid_t out;

  ASSERT_EQ(-1, uuid_parse("000000000000-0000-0000-000000000000 ", out));
  ASSERT_EQ(-1, uuid_parse("00000000-00000000-0000-000000000000 ", out));
  ASSERT_EQ(-1, uuid_parse("00000000-0000-00000000-000000000000 ", out));
  ASSERT_EQ(-1, uuid_parse("00000000-0000-0000-0000000000000000 ", out));
  ASSERT_EQ(0, uuid_parse("01234567-89ab-cdef-0123-123412341234", out));
  ASSERT_EQ(0, memcmp(exp, out, sizeof(uuid_t)));
}

TEST_F(UUIDTest, uuid_unparse) {
  uuid_t in = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                0x01, 0x23, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34};
  uuid_string_t out, exp = "01234567-89ab-cdef-0123-123412341234";

  uuid_unparse(in, out);
  ASSERT_EQ(0, strcmp(out, exp));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
