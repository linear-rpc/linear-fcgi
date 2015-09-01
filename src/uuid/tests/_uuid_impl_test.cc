#include <stdio.h>
#include <string.h>
#include <gtest/gtest.h>

#include "_uuid_clock.h"
#include "_uuid_node.h"
#include "_uuid_impl.h"

using namespace std;

class _UUIDImplTest : public testing::Test {
protected:
  _UUIDImplTest() {}
  ~_UUIDImplTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(_UUIDImplTest, _uuid_generate) {
  uuid_node_t node;
  uint8_t seq_hi, seq_low;
  _uuid_t uu;

  ASSERT_EQ(0, _uuid_get_node(node));
  _uuid_get_clock(&seq_low, &seq_hi);

  ASSERT_DEATH(_uuid_generate(NULL), "");
  ASSERT_EQ(0, _uuid_generate(&uu));
  ASSERT_EQ(0, memcmp(uu.node, node, sizeof(node)));
  if (uu.clock_seq_low == 0) {
    ASSERT_EQ((seq_hi + 1) | 0x80, uu.clock_seq_hi_and_reserved);
  } else {
    ASSERT_EQ(seq_low + 1, uu.clock_seq_low);
    ASSERT_EQ(seq_hi | 0x80, uu.clock_seq_hi_and_reserved);
  }
}

TEST_F(_UUIDImplTest, _uuid_compare) {
  _uuid_t uu1, uu2;

  ASSERT_DEATH(_uuid_compare(NULL, &uu2), "");
  ASSERT_DEATH(_uuid_compare(&uu1, NULL), "");

  memset(&uu1, 0, sizeof(uu1));
  memset(&uu2, 0, sizeof(uu2));

  uu1.time_low = 0;
  uu2.time_low = 1;
  ASSERT_GT(0, _uuid_compare(&uu1, &uu2));
  uu1.time_low = 1;
  uu2.time_low = 0;
  ASSERT_LT(0, _uuid_compare(&uu1, &uu2));
  uu1.time_low = 0;
  uu2.time_low = 0;

  uu1.time_mid = 0;
  uu2.time_mid = 1;
  ASSERT_GT(0, _uuid_compare(&uu1, &uu2));
  uu1.time_mid = 1;
  uu2.time_mid = 0;
  ASSERT_LT(0, _uuid_compare(&uu1, &uu2));
  uu1.time_mid = 0;
  uu2.time_mid = 0;

  uu1.time_hi_and_version = 0;
  uu2.time_hi_and_version = 1;
  ASSERT_GT(0, _uuid_compare(&uu1, &uu2));
  uu1.time_hi_and_version = 1;
  uu2.time_hi_and_version = 0;
  ASSERT_LT(0, _uuid_compare(&uu1, &uu2));
  uu1.time_hi_and_version = 0;
  uu2.time_hi_and_version = 0;

  uu1.clock_seq_hi_and_reserved = 0;
  uu2.clock_seq_hi_and_reserved = 1;
  ASSERT_GT(0, _uuid_compare(&uu1, &uu2));
  uu1.clock_seq_hi_and_reserved = 1;
  uu2.clock_seq_hi_and_reserved = 0;
  ASSERT_LT(0, _uuid_compare(&uu1, &uu2));
  uu1.clock_seq_hi_and_reserved = 0;
  uu2.clock_seq_hi_and_reserved = 0;

  uu1.clock_seq_low = 0;
  uu2.clock_seq_low = 1;
  ASSERT_GT(0, _uuid_compare(&uu1, &uu2));
  uu1.clock_seq_low = 1;
  uu2.clock_seq_low = 0;
  ASSERT_LT(0, _uuid_compare(&uu1, &uu2));
  uu1.clock_seq_low = 0;
  uu2.clock_seq_low = 0;

  memset(uu1.node, 0, sizeof(uu1.node));
  memset(uu2.node, 1, sizeof(uu1.node));
  ASSERT_GT(0, _uuid_compare(&uu1, &uu2));
  memset(uu1.node, 1, sizeof(uu1.node));
  memset(uu2.node, 0, sizeof(uu1.node));
  ASSERT_LT(0, _uuid_compare(&uu1, &uu2));
  memset(uu1.node, 0, sizeof(uu1.node));
  memset(uu2.node, 0, sizeof(uu1.node));

  ASSERT_EQ(0, _uuid_compare(&uu1, &uu2));
}

TEST_F(_UUIDImplTest, _uuid_parse) {
  uuid_node_t node = {0x12, 0x34, 0x12, 0x34, 0x12, 0x34};
  _uuid_t uu;

  ASSERT_DEATH(_uuid_parse("12341234-1234-1234-1234-123412341234", NULL), "");
  ASSERT_EQ(-1, _uuid_parse("000000000000-0000-0000-000000000000 ", &uu));
  ASSERT_EQ(-1, _uuid_parse("00000000-00000000-0000-000000000000 ", &uu));
  ASSERT_EQ(-1, _uuid_parse("00000000-0000-00000000-000000000000 ", &uu));
  ASSERT_EQ(-1, _uuid_parse("00000000-0000-0000-0000000000000000 ", &uu));
  ASSERT_EQ(0, _uuid_parse("12341234-1234-1234-1234-123412341234", &uu));
  ASSERT_EQ((uint32_t)0x12341234, uu.time_low);
  ASSERT_EQ(0x1234, uu.time_mid);
  ASSERT_EQ(0x1234, uu.time_hi_and_version);
  ASSERT_EQ(0x12, uu.clock_seq_hi_and_reserved);
  ASSERT_EQ(0x34, uu.clock_seq_low);
  ASSERT_EQ(0, memcmp(uu.node, node, sizeof(node)));
}

TEST_F(_UUIDImplTest, _uuid_unparse) {
  uuid_node_t node = {0x12, 0x34, 0x12, 0x34, 0x12, 0x34};
  _uuid_t uu;
  uuid_string_t out, exp = "12341234-1234-1234-1234-123412341234";

  uu.time_low = 0x12341234;
  uu.time_mid = 0x1234;
  uu.time_hi_and_version = 0x1234;
  uu.clock_seq_hi_and_reserved = 0x12;
  uu.clock_seq_low = 0x34;
  memcpy(uu.node, node, sizeof(node));

  ASSERT_DEATH(_uuid_unparse(NULL, out), "");
  _uuid_unparse(&uu, out);
  ASSERT_EQ(0, strcmp(out, exp));
}

TEST_F(_UUIDImplTest, _uuid_pack) {
  uuid_node_t node = {0x12, 0x34, 0x12, 0x34, 0x12, 0x34};
  _uuid_t src;
  uuid_t dst;

  src.time_low = 0x01234567;
  src.time_mid = 0x89ab;
  src.time_hi_and_version = 0xcdef;
  src.clock_seq_hi_and_reserved = 0x01;
  src.clock_seq_low = 0x23;
  memcpy(src.node, node, sizeof(node));

  ASSERT_DEATH(_uuid_pack(dst, NULL), "");
  _uuid_pack(dst, &src);
  ASSERT_EQ(0x01, dst[0]);
  ASSERT_EQ(0x23, dst[1]);
  ASSERT_EQ(0x45, dst[2]);
  ASSERT_EQ(0x67, dst[3]);
  ASSERT_EQ(0x89, dst[4]);
  ASSERT_EQ(0xab, dst[5]);
  ASSERT_EQ(0xcd, dst[6]);
  ASSERT_EQ(0xef, dst[7]);
  ASSERT_EQ(0x01, dst[8]);
  ASSERT_EQ(0x23, dst[9]);
  ASSERT_EQ(0, memcmp(&dst[10], node, sizeof(node)));
}

TEST_F(_UUIDImplTest, _uuid_unpack) {
  uuid_node_t node = {0x12, 0x34, 0x12, 0x34, 0x12, 0x34};
  uuid_t src = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                0x01, 0x23, 0x12, 0x34, 0x12, 0x34, 0x12, 0x34};
  _uuid_t dst;

  dst.time_low = 0x01234567;
  dst.time_mid = 0x89ab;
  dst.time_hi_and_version = 0xcdef;
  dst.clock_seq_hi_and_reserved = 0x01;
  dst.clock_seq_low = 0x23;
  memcpy(dst.node, node, sizeof(node));

  ASSERT_DEATH(_uuid_unpack(NULL, src), "");
  _uuid_unpack(&dst, src);
  ASSERT_EQ((uint32_t)0x01234567, dst.time_low);
  ASSERT_EQ(0x89ab, dst.time_mid);
  ASSERT_EQ(0xcdef, dst.time_hi_and_version);
  ASSERT_EQ(0x01, dst.clock_seq_hi_and_reserved);
  ASSERT_EQ(0x23, dst.clock_seq_low);
  ASSERT_EQ(0, memcmp(dst.node, node, sizeof(node)));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
