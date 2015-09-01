// fcgi_test

#include <gtest/gtest.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "linear_fcgi_env.h"

using namespace linear::fcgi;

class LinearFcgiEnvTest : public testing::Test {
protected:
  LinearFcgiEnvTest() {}
  ~LinearFcgiEnvTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(LinearFcgiEnvTest, get) {
#define LINEAR_FCGI_ENV_ENUM_DECL_GEN(IDENT, STRING)   \
  Env::Ident ENUM_##IDENT = Env::IDENT;

  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

#define LINEAR_FCGI_ENV_MAP_GEN(IDENT, STRING) \
  envs.insert(std::map<Env::Ident, std::string>::value_type(ENUM_##IDENT, STRING));

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  const char* TEST_STRING = "TEST_STRING";
  std::string result;
  std::string backup;

  std::map<Env::Ident, std::string>::iterator it = envs.begin();
  while (it != envs.end()) {
    backup = Env::get((*it).first);
    ASSERT_EQ(0, unsetenv((*it).second.c_str()));
    ASSERT_EQ(0, setenv((*it).second.c_str(), TEST_STRING, 1));
    result = Env::get((*it).first);
    std::cerr << "[" << std::setw(16) << std::left << (*it).second << "] = "
              << result << std::endl;
    ASSERT_STREQ(TEST_STRING, result.c_str());
    ASSERT_EQ(0, setenv((*it).second.c_str(), backup.c_str(), 1));
    ++it;
  }
#undef FCGI_ENV_MAP_GEN
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
