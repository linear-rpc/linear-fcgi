#ifndef _LINEAR_FCGI_REQUEST_H_
#define _LINEAR_FCGI_REQUEST_H_

#include <string>
#include <map>

#include "linear_fcgi_env.h"
#include "linear_fcgi_http.h"
#include "linear_fcgi_queries.h"
#include "linear_fcgi_response.h"

namespace linear {
namespace fcgi {

#define LINEAR_FCGI_REQUEST_METHOD_MAP(GEN)     \
  GEN(GET, "GET")                               \
  GEN(POST, "POST")

class Request {
public:
#define LINEAR_FCGI_REQUEST_ENUM_GEN(METHOD, STRING) METHOD,
  enum Method {
    LINEAR_FCGI_REQUEST_METHOD_MAP(LINEAR_FCGI_REQUEST_ENUM_GEN)
    UNKNOWN
  };
#undef LINEAR_FCGI_REQUEST_ENUM_GEN

public:
  static const Request create();

public:
  Request() : queries_("") {}
 ~Request() {}

public:
  Method getMethod() const;
  const std::string& getRemoteHost() const;
  const std::string& getRemoteAddr() const;
  const std::string& getRemotePort() const;
  const std::string& getContentType() const;
  const std::string& getContentLength() const;
  const std::string& getCookie() const;
  const std::string& getOrigin() const;
  const std::string& getUserAgent() const;
  const Queries& getQueries() const;

  int sendResponse(Response::Code code) const;
  int sendResponse(Response::Code code, const std::string& body) const;

private:
  const std::string& getQueryString() const;

private:
  std::map<Env::Ident, std::string> envs_;
  Queries queries_;
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_REQUEST_HPP_
