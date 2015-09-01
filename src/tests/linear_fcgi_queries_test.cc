// fcgi_test

#include <gtest/gtest.h>
#include <stdlib.h>

#include <string>
#include <sstream>

#include "linear_fcgi_queries.h"

using namespace linear::fcgi;

class LinearFcgiQueriesTest : public testing::Test {
protected:
  LinearFcgiQueriesTest() {}
  ~LinearFcgiQueriesTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

#define LINEAR_FCGI_QUERY_TEST_MAP(GEN)              \
  GEN(SESSION_ID, "sid", "00000000-0000-0000-0000-000000000000") \
  GEN(CHANNEL, "ch", "/TEST_CHANNEL")                         \
  GEN(COMMAND, "c", "TEST_COMMAND")                           \
  GEN(DATA, "data", "TEST_DATA")                           \
  GEN(CALLBACK, "f", "TEST_CALLBACK")

TEST_F(LinearFcgiQueriesTest, construct) {
#define LINEAR_FCGI_QUERY_TEST_RESULT_GEN(IDENT, KEY, VAL) const char IDENT[] = VAL;
#define LINEAR_FCGI_QUERY_TEST_STRING_GEN(IDENT, KEY, VAL) os << KEY << "=" << VAL << "&";

  LINEAR_FCGI_QUERY_TEST_MAP(LINEAR_FCGI_QUERY_TEST_RESULT_GEN)
  std::ostringstream os;
  LINEAR_FCGI_QUERY_TEST_MAP(LINEAR_FCGI_QUERY_TEST_STRING_GEN);

  Queries q0(std::string(""));
  ASSERT_STREQ("", q0.getSessionId().c_str());
  ASSERT_STREQ("", q0.getChannel().c_str());
  ASSERT_STREQ("", q0.getCommand().c_str());
  ASSERT_STREQ("", q0.getData().c_str());
  ASSERT_STREQ("", q0.getCallback().c_str());

  Queries q1(os.str());
  ASSERT_STREQ(SESSION_ID, q1.getSessionId().c_str());
  ASSERT_STREQ(CHANNEL, q1.getChannel().c_str());
  ASSERT_STREQ(COMMAND, q1.getCommand().c_str());
  ASSERT_STREQ(DATA, q1.getData().c_str());
  ASSERT_STREQ(CALLBACK, q1.getCallback().c_str());

#undef LINEAR_FCGI_QUERY_TEST_RESULT_GEN
#undef LINEAR_FCGI_QUERY_TEST_STRING_GEN
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
