// fcgi_test

#include <gtest/gtest.h>
#include <stdlib.h>

#include <map>
#include <string>
#include <sstream>

#include "linear_fcgi_response.h"

using namespace linear::fcgi;

class LinearFcgiResponseTest : public testing::Test {
protected:
  LinearFcgiResponseTest() {}
  ~LinearFcgiResponseTest() {}
  virtual void SetUp() {}
  virtual void TearDown() {}
};

#define LINEAR_FCGI_RESPONSE_ENUM_GEN(CODE, NUM, STRING)        \
  Response::Code RESPONSE_##CODE = Response::CODE;

#define LINEAR_FCGI_MIMETYPE_ENUM_GEN(IDENT, STRING)    \
  Http::MimeType MIME_##IDENT = Http::IDENT;

TEST_F(LinearFcgiResponseTest, createCodeOnly) {
  LINEAR_FCGI_RESPONSE_MAP(LINEAR_FCGI_RESPONSE_ENUM_GEN);

#define LINEAR_FCGI_RESPONSE_MAP_GEN(CODE, NUM, STRING) \
  os.str(""); os.clear(); \
  os << "Status: " << NUM << "\r\n\r\n"; \
  resps.insert(std::map<Response::Code, std::string>::value_type(RESPONSE_##CODE, os.str()));

  std::ostringstream os;
  std::map<Response::Code, std::string> resps;
  LINEAR_FCGI_RESPONSE_MAP(LINEAR_FCGI_RESPONSE_MAP_GEN);

  std::string result;

  std::map<Response::Code, std::string>::iterator it = resps.begin();
  while (it != resps.end()) {
    result = Response::create((*it).first);
    std::cerr << "Code: " << (*it).first
              << "\nExpect: [" << (*it).second << "]"
              << "\nResult: [" << result << "]" << std::endl;
    ASSERT_STREQ((*it).second.c_str(), result.c_str());
    ++it;
  }
#undef LINEAR_FCGI_RESPONSE_MAP_GEN
}

TEST_F(LinearFcgiResponseTest, createWithBody) {
  LINEAR_FCGI_RESPONSE_MAP(LINEAR_FCGI_RESPONSE_ENUM_GEN);
  LINEAR_FCGI_MIME_TYPE_MAP(LINEAR_FCGI_MIMETYPE_ENUM_GEN);

#define LINEAR_FCGI_STATUS_LINE_GEN(CODE, NUM, STRING)                  \
  os.str(""); os.clear();                                               \
  os << "Status: " << NUM << "\r\n";                                    \
  stats.insert(std::map<Response::Code, std::string>::value_type(RESPONSE_##CODE, os.str()));

#define LINEAR_FCGI_CONTENT_TYPE_GEN(IDENT, STRING)    \
  os.str(""); os.clear();                              \
  os << "Content-Type: " << STRING << "\r\n";          \
  ctypes.insert(std::map<Http::MimeType, std::string>::value_type(MIME_##IDENT, os.str()));

  std::ostringstream os;
  std::map<Response::Code, std::string> stats;
  std::map<Http::MimeType, std::string> ctypes;

  LINEAR_FCGI_RESPONSE_MAP(LINEAR_FCGI_STATUS_LINE_GEN);
  LINEAR_FCGI_MIME_TYPE_MAP(LINEAR_FCGI_CONTENT_TYPE_GEN);

  std::string exp;
  std::string res;
  std::string body = "TEST_BODY";

  // type == default
  std::map<Response::Code, std::string>::iterator sit = stats.begin();
  while (sit != stats.end()) {
    res = Response::create((*sit).first, body);
    exp = (*sit).second + "Content-Type: application/json\r\n";
    os.str(""); os.clear();
    os << "Content-Length: " << body.size() << "\r\n\r\n" << body;
    exp += os.str();
    std::cerr << "\nExpect: [" << exp << "]"
              << "\nResult: [" << res << "]" << std::endl;
    ASSERT_STREQ(exp.c_str(), res.c_str());
    ++sit;
  }

  sit = stats.begin();
  while (sit != stats.end()) {
    std::map<Http::MimeType, std::string>::iterator cit = ctypes.begin();
    while (cit != ctypes.end()) {
      res = Response::create((*sit).first, body, (*cit).first);
      exp = (*sit).second + (*cit).second;
      os.str(""); os.clear();
      os << "Content-Length: " << body.size() << "\r\n\r\n" << body;
      exp += os.str();
      std::cerr << "\nExpect: [" << exp << "]"
                << "\nResult: [" << res << "]" << std::endl;
      ASSERT_STREQ(exp.c_str(), res.c_str());
      ++cit;
    }
    ++sit;
  }
#undef LINEAR_FCGI_CONTENT_TYPE_GEN
#undef LINEAR_FCGI_STATUS_LINE_GEN
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
