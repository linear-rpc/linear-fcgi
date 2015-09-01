#ifndef _LINEAR_FCGI_ENV_H_
#define _LINEAR_FCGI_ENV_H_

namespace linear {
namespace fcgi {

#define LINEAR_FCGI_ENV_MAP(GEN)                \
  GEN(REQUEST_METHOD, "REQUEST_METHOD")         \
  GEN(REMOTE_HOST, "REMOTE_HOST")               \
  GEN(REMOTE_ADDR, "REMOTE_ADDR")               \
  GEN(REMOTE_PORT, "REMOTE_PORT")               \
  GEN(CONTENT_TYPE, "CONTENT_TYPE")             \
  GEN(CONTENT_LENGTH, "CONTENT_LENGTH")         \
  GEN(HTTP_COOKIE, "HTTP_COOKIE")               \
  GEN(HTTP_ORIGIN, "HTTP_ORIGIN")               \
  GEN(HTTP_USER_AGENT, "HTTP_USER_AGENT")       \
  GEN(QUERY_STRING, "QUERY_STRING")

class Env {
public:
#define LINEAR_FCGI_ENV_ENUM_GEN(IDENT, STRING) IDENT,
  enum Ident {
    LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_GEN)
    END_OF_IDENT
  };
#undef LINEAR_FCGI_ENV_ENUM_GEN

public:
  Env() {}
  ~Env() {}
  static const std::string get(Ident ident);
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_ENV_H_
