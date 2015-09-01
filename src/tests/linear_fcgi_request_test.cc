// fcgi_request_test

#include <gtest/gtest.h>
#include <stdlib.h>

#include "fcgi_stdio.h"

#include <map>
#include <string>
#include <sstream>

#include "linear_fcgi_request.h"

#define LINEAR_FCGI_ENV_ENUM_DECL_GEN(IDENT, STRING) Env::Ident ENUM_##IDENT = Env::IDENT;

#define LINEAR_FCGI_ENV_MAP_GEN(IDENT, STRING) \
  envs.insert(std::map<Env::Ident, std::string>::value_type(ENUM_##IDENT, STRING));

using namespace linear::fcgi;

class FcgiRequestTest : public testing::Test {
protected:
  FcgiRequestTest() {}
  ~FcgiRequestTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(FcgiRequestTest, create) {
  Request req;
  ASSERT_DEATH(req.getRemoteHost(), "");
  ASSERT_DEATH(req.getRemoteAddr(), "");
  ASSERT_DEATH(req.getContentType(), "");
  ASSERT_DEATH(req.getContentLength(), "");
  ASSERT_DEATH(req.getCookie(), "");
  ASSERT_DEATH(req.getOrigin(), "");
  ASSERT_DEATH(req.getUserAgent(), "");

  req = Request::create();
  ASSERT_STRNE(NULL, req.getRemoteHost().c_str());
  ASSERT_STRNE(NULL, req.getRemoteAddr().c_str());
  ASSERT_STRNE(NULL, req.getContentType().c_str());
  ASSERT_STRNE(NULL, req.getContentLength().c_str());
  ASSERT_STRNE(NULL, req.getCookie().c_str());
  ASSERT_STRNE(NULL, req.getOrigin().c_str());
  ASSERT_STRNE(NULL, req.getUserAgent().c_str());
}

TEST_F(FcgiRequestTest, getMethod) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::map<Env::Ident, std::string>::iterator it = envs.find(Env::REQUEST_METHOD);

  // malformed test
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  req = Request::create();
  ASSERT_EQ(Request::UNKNOWN, req.getMethod());

  // GET test
  const char* GET_STRING = "GET";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), GET_STRING, 1));
  req = Request::create();
  ASSERT_EQ(Request::GET, req.getMethod());

  // POST test
  const char* POST_STRING = "POST";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), POST_STRING, 1));
  req = Request::create();
  ASSERT_EQ(Request::POST, req.getMethod());

  // HEAD test => UNKNOWN
  const char* HEAD_STRING = "HEAD";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), HEAD_STRING, 1));
  req = Request::create();
  ASSERT_EQ(Request::UNKNOWN, req.getMethod());
}

TEST_F(FcgiRequestTest, getRemoteHost) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::map<Env::Ident, std::string>::iterator it = envs.find(Env::REMOTE_HOST);

  // malformed test
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  req = Request::create();
  ASSERT_STREQ("", req.getRemoteHost().c_str());

  // GET test
  const char* TEST_STRING = "TEST_STRING";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), TEST_STRING, 1));
  req = Request::create();
  ASSERT_STREQ(TEST_STRING, req.getRemoteHost().c_str());
}

TEST_F(FcgiRequestTest, getRemoteAddr) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::map<Env::Ident, std::string>::iterator it = envs.find(Env::REMOTE_ADDR);

  // malformed test
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  req = Request::create();
  ASSERT_STREQ("", req.getRemoteAddr().c_str());

  // GET test
  const char* TEST_STRING = "TEST_STRING";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), TEST_STRING, 1));
  req = Request::create();
  ASSERT_STREQ(TEST_STRING, req.getRemoteAddr().c_str());
}

TEST_F(FcgiRequestTest, getContentType) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::map<Env::Ident, std::string>::iterator it = envs.find(Env::CONTENT_TYPE);

  // malformed test
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  req = Request::create();
  ASSERT_STREQ("", req.getContentType().c_str());

  // GET test
  const char* TEST_STRING = "TEST_STRING";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), TEST_STRING, 1));
  req = Request::create();
  ASSERT_STREQ(TEST_STRING, req.getContentType().c_str());
}

TEST_F(FcgiRequestTest, getContentLength) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::map<Env::Ident, std::string>::iterator it = envs.find(Env::CONTENT_LENGTH);

  // malformed test
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  req = Request::create();
  ASSERT_STREQ("", req.getContentLength().c_str());

  // GET test
  const char* TEST_STRING = "TEST_STRING";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), TEST_STRING, 1));
  req = Request::create();
  ASSERT_STREQ(TEST_STRING, req.getContentLength().c_str());
}

TEST_F(FcgiRequestTest, getCookie) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::map<Env::Ident, std::string>::iterator it = envs.find(Env::HTTP_COOKIE);

  // malformed test
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  req = Request::create();
  ASSERT_STREQ("", req.getCookie().c_str());

  // GET test
  const char* TEST_STRING = "TEST_STRING";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), TEST_STRING, 1));
  req = Request::create();
  ASSERT_STREQ(TEST_STRING, req.getCookie().c_str());
}

TEST_F(FcgiRequestTest, getOrigin) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::map<Env::Ident, std::string>::iterator it = envs.find(Env::HTTP_ORIGIN);

  // malformed test
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  req = Request::create();
  ASSERT_STREQ("", req.getOrigin().c_str());

  // GET test
  const char* TEST_STRING = "TEST_STRING";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), TEST_STRING, 1));
  req = Request::create();
  ASSERT_STREQ(TEST_STRING, req.getOrigin().c_str());
}

TEST_F(FcgiRequestTest, getUserAgent) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::map<Env::Ident, std::string>::iterator it = envs.find(Env::HTTP_USER_AGENT);

  // malformed test
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  req = Request::create();
  ASSERT_STREQ("", req.getUserAgent().c_str());

  // GET test
  const char* TEST_STRING = "TEST_STRING";
  ASSERT_EQ(0, unsetenv((*it).second.c_str()));
  ASSERT_EQ(0, setenv((*it).second.c_str(), TEST_STRING, 1));
  req = Request::create();
  ASSERT_STREQ(TEST_STRING, req.getUserAgent().c_str());
}

TEST_F(FcgiRequestTest, getQueries) {
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);

  std::map<Env::Ident, std::string> envs;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  Request req;
  std::string method = (*envs.find(Env::REQUEST_METHOD)).second;
  std::string content_length = (*envs.find(Env::CONTENT_LENGTH)).second;
  std::string query_string = (*envs.find(Env::QUERY_STRING)).second;

  // malformed test
  ASSERT_EQ(0, unsetenv(method.c_str()));
  req = Request::create();
  ASSERT_EQ(Request::UNKNOWN, req.getMethod());
  ASSERT_STREQ("", req.getQueries().getSessionId().c_str());
  ASSERT_STREQ("", req.getQueries().getCommand().c_str());

  ASSERT_EQ(0, setenv(method.c_str(), "GET", 1));
  req = Request::create();
  ASSERT_EQ(Request::GET, req.getMethod());
  ASSERT_STREQ("", req.getQueries().getSessionId().c_str());
  ASSERT_STREQ("", req.getQueries().getCommand().c_str());

  // get method
  std::string test_string = "sid=00000000-0000-0000-0000-000000000000&c=poll&0123456789";
  ASSERT_EQ(0, setenv(method.c_str(), "GET", 1));
  ASSERT_EQ(0, setenv(query_string.c_str(), test_string.c_str(), 1));

  req = Request::create();
  ASSERT_STREQ("00000000-0000-0000-0000-000000000000", req.getQueries().getSessionId().c_str());
  ASSERT_STREQ("poll", req.getQueries().getCommand().c_str());

  // post method
  ASSERT_EQ(0, setenv(method.c_str(), "POST", 1));
  size_t siz = test_string.size();
  std::ostringstream os;
  os << siz;
  ASSERT_EQ(0, setenv(content_length.c_str(), os.str().c_str(), 1));
  FCGI_FILE* fp = FCGI_fopen("test.txt", "w+");
  FCGI_fwrite(const_cast<char*>(test_string.c_str()), test_string.size(), 1, fp);
  FCGI_fseek(fp, 0, SEEK_SET);
  _fcgi_sF[0].stdio_stream = fp->stdio_stream;

  req = Request::create();
  ASSERT_STREQ("00000000-0000-0000-0000-000000000000", req.getQueries().getSessionId().c_str());
  ASSERT_STREQ("poll", req.getQueries().getCommand().c_str());
  _fcgi_sF[0].stdio_stream = 0;
  FCGI_fclose(fp);
  unlink("test.txt");
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#undef LINEAR_FCGI_ENV_MAP_GEN
#undef LINEAR_FCGI_ENV_ENUM_DECL_GEN
