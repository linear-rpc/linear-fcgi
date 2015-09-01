#include <stdio.h>
#include <string.h>
#include <gtest/gtest.h>

#include "_uuid_node.h"

using namespace std;

class _UUIDNodeTest : public testing::Test {
protected:
  _UUIDNodeTest() {}
  ~_UUIDNodeTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(_UUIDNodeTest, _uuid_get_node) {
  uuid_node_t node;
  char mac[32];

  if (_uuid_get_node(node) < 0) {
    FAIL() << "fail to get mac addr.";
  }
  memset(mac, 0, sizeof(mac));
  snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x\n",
          node[0], node[1], node[2], node[3], node[4], node[5]);
  fprintf(stderr, "%s", mac);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
