#include <gtest/gtest.h>
#include <stdio.h>

#include "urlendecode.h"

using namespace std;

class URLEnDecodeTest : public testing::Test {
protected:
  URLEnDecodeTest() {}
  ~URLEnDecodeTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(URLEnDecodeTest, url_encode) {
  char *result;

  ASSERT_DEATH(url_encode(NULL), "");
  result = url_encode("0123456789");
  ASSERT_STRNE(NULL, result);
  free(result);

  const char *exp1 = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_.";
  result = url_encode(exp1);
  ASSERT_STREQ(exp1, result);
  free(result);

  const char *exp2 = "+";
  result = url_encode(" ");
  ASSERT_STREQ(exp2, result);
  free(result);

  const char *exp3 = "%40%2f%5b%5d%7b%7d%28%29%2a%2b%3d%2c%3a%3b%5c%7c%3f%21%22%23%24%25%26%27%5e%7e%60";
  result = url_encode("@/[]{}()*+=,:;\\|?!\"#$%&'^~`");
  ASSERT_STREQ(exp3, result);
  free(result);
}

TEST_F(URLEnDecodeTest, url_decode) {
  char *result;

  ASSERT_DEATH(url_encode(NULL), "");
  result = url_encode("0123456789");
  ASSERT_STRNE(NULL, result);
  free(result);

  const char *exp1 = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_.";
  result = url_decode(exp1);
  ASSERT_STREQ(exp1, result);
  free(result);

  const char *exp2 = " ";
  result = url_decode("+");
  ASSERT_STREQ(exp2, result);
  free(result);

  const char *exp3 = "@/[]{}()*+=,:;\\|?!\"#$%&'^~`";
  result = url_decode("%40%2f%5b%5d%7b%7d%28%29%2a%2b%3d%2c%3a%3b%5c%7c%3f%21%22%23%24%25%26%27%5e%7e%60");
  ASSERT_STREQ(exp3, result);
  free(result);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
