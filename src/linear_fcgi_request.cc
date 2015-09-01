#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <iostream>

#include "fcgi_stdio.h"
#include "linear_fcgi_request.h"

namespace linear {
namespace fcgi {

const Request Request::create() {
#define LINEAR_FCGI_ENV_ENUM_DECL_GEN(IDENT, STRING) Env::Ident ENUM_##IDENT = Env::IDENT;
#define LINEAR_FCGI_ENV_MAP_GEN(IDENT, STRING) \
  req.envs_.insert(std::map<Env::Ident, std::string>::value_type(ENUM_##IDENT, \
                                                                 Env::get(ENUM_##IDENT)));
  Request req;
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_ENUM_DECL_GEN);
  LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_MAP_GEN);

  req.queries_ = Queries(req.getQueryString());
  return req;

#undef LINEAR_FCGI_ENV_MAP_GEN
#undef LINEAR_FCGI_ENV_ENUM_DECL_GEN
}

Request::Method Request::getMethod() const {
#define LINEAR_FCGI_REQUEST_METHOD_ENUM_DECL_GEN(IDENT, STRING) \
  Request::Method ENUM_##IDENT = Request::IDENT;
#define LINEAR_FCGI_REQUEST_METHOD_GEN(IDENT, STRING) \
  if ( (*(envs_.find(Env::REQUEST_METHOD))).second == STRING ) { return ENUM_##IDENT; }

  LINEAR_FCGI_REQUEST_METHOD_MAP(LINEAR_FCGI_REQUEST_METHOD_ENUM_DECL_GEN);
  LINEAR_FCGI_REQUEST_METHOD_MAP(LINEAR_FCGI_REQUEST_METHOD_GEN);
  return Request::UNKNOWN;

#undef LINEAR_FCGI_REQUEST_METHOD_ENUM_DECL_GEN
#undef LINEAR_FCGI_REQUEST_METHOD_GEN
}

#define FCGI_REQUEST_ENV_FIND(IDENT)                                    \
  std::map<Env::Ident, std::string>::const_iterator it = envs_.find(IDENT); \
  if (it == envs_.end()) {                                              \
    assert(false); /* IDENT must be existed */                          \
  }                                                                     \
  return (*it).second;

const std::string& Request::getRemoteHost() const {
  FCGI_REQUEST_ENV_FIND(Env::REMOTE_HOST);
}

const std::string& Request::getRemoteAddr() const {
  FCGI_REQUEST_ENV_FIND(Env::REMOTE_ADDR);
}

const std::string& Request::getRemotePort() const {
  FCGI_REQUEST_ENV_FIND(Env::REMOTE_PORT);
}

const std::string& Request::getContentType() const {
  FCGI_REQUEST_ENV_FIND(Env::CONTENT_TYPE);
}

const std::string& Request::getContentLength() const {
  FCGI_REQUEST_ENV_FIND(Env::CONTENT_LENGTH);
}

const std::string& Request::getCookie() const {
  FCGI_REQUEST_ENV_FIND(Env::HTTP_COOKIE);
}

const std::string& Request::getOrigin() const {
  FCGI_REQUEST_ENV_FIND(Env::HTTP_ORIGIN);
}

const std::string& Request::getUserAgent() const {
  FCGI_REQUEST_ENV_FIND(Env::HTTP_USER_AGENT);
}

const Queries& Request::getQueries() const {
  return queries_;
}

const std::string& Request::getQueryString() const {
  static std::string data("");
  Method method = getMethod();

  if (method == GET) {
    FCGI_REQUEST_ENV_FIND(Env::QUERY_STRING);
  } else if (method == POST) {
    std::string content_length = getContentLength();
    char* query_string = NULL;
    size_t siz;

    siz = (size_t)strtol(content_length.c_str(), NULL, 10);
    if (errno == ERANGE) {
      return data;
    }
    query_string = (char*)malloc(siz + 1);
    if (!query_string) {
      return data;
    }
    memset(query_string, 0, siz);
    if (!FCGI_fread(query_string, siz, 1, FCGI_stdin)) {
      free(query_string);
      return data;
    }
    query_string[siz] = '\0';
    data = query_string;
    free(query_string);
  }
  return data;
}

#undef FCGI_REQUEST_ENV_FIND

int Request::sendResponse(Response::Code code) const {
  std::string str = Response::create(code);
  return FCGI_printf("%s", str.c_str());
}

int Request::sendResponse(Response::Code code, const std::string& body) const {
  std::string str = Response::create(code, body);
  return FCGI_printf("%s", str.c_str());
}

} // namespace fcgi
} // namespace linear

