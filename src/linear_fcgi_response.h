#ifndef _LINEAR_FCGI_RESPONSE_H_
#define _LINEAR_FCGI_RESPONSE_H_

#include <string>

#include "linear_fcgi_http.h"

namespace linear {
namespace fcgi {

#define LINEAR_FCGI_RESPONSE_MAP(GEN)                                   \
  GEN(OK, 200, "OK")                                                    \
  GEN(CREATED, 201, "Created")                                          \
  GEN(ACCEPTED, 202, "Accepted")                                        \
  GEN(NON_AUTHORITATIVE_INFORMATION, 203, "Non-Authoritative Information") \
  GEN(NO_CONTENT, 204, "No Content")                                    \
  GEN(RESET_CONTENT, 205, "Reset Content")                              \
  GEN(PARTIAL_CONTENT, 206, "Partial Content")                          \
  GEN(MULTIPLE_CHOICES, 300, "Multiple Choices")                        \
  GEN(MOVED_PERMANENTLY, 301, "Moved Permanently")                      \
  GEN(FOUND, 302, "Found")                                              \
  GEN(SEE_OTHER, 303, "See Other")                                      \
  GEN(NOT_MODIFIED, 304, "Not Modified")                                \
  GEN(USE_PROXY, 305, "Use Proxy")                                      \
  GEN(BAD_REQUEST, 400, "Bad Request")                                  \
  GEN(UNAUTHORIZED, 401, "Unauthorized")                                \
  GEN(PAYMENT_REQUIRED, 402, "Payment Required")                        \
  GEN(FORBIDDEN, 403, "Forbidden")                                      \
  GEN(NOT_FOUND, 404, "Not Found")                                      \
  GEN(METHOD_NOT_ALLOWED, 405, "Method Not Allowed")                    \
  GEN(NOT_ACCEPTABLE, 406, "Not Acceptable")                            \
  GEN(PROXY_AUTHENTICATION_REQUIRED, 407, "Proxy Authentication Required") \
  GEN(REQUEST_TIMEOUT, 408, "Request Timeout")                          \
  GEN(CONFLICT, 409, "Conflict")                                        \
  GEN(GONE, 410, "Gone")                                                \
  GEN(LENGTH_REQUIRED, 411, "Length Required")                          \
  GEN(PRECONDITION_FAILD, 412, "Precondition Failed")                   \
  GEN(REQUEST_ENTITY_TOO_LARGE, 413, "Request Entity Too Large")        \
  GEN(REQUEST_URI_TOO_LONG, 414, "Request-URI Too Long")                \
  GEN(UNSUPPORTED_MEDIA_TYPE, 415, "Unsupported Media Type")            \
  GEN(REQUEST_RANGE_NOT_SATISFIABLE, 416, "Request Range Not Satisfiable") \
  GEN(EXPECTATION_FAILED, 417, "Expectation Failed")                    \
  GEN(INTERNAL_SERVER_ERROR, 500, "Internal Server Error")              \
  GEN(NOT_IMPLEMENTED, 501, "Not Implemented")                          \
  GEN(BAD_GATEWAY, 502, "Bad Gateway")                                  \
  GEN(SERVICE_UNAVAILABLE, 503, "Servide Unavailable")                  \
  GEN(GATEWAY_TIMEOUT, 504, "Gateway Timeout")                          \
  GEN(HTTP_VERSION_NOT_SUPPORTED, 505, "HTTP Version Not Supported")

class Response {
public:
#define LINEAR_FCGI_RESPONSE_ENUM_GEN(CODE, NUM, STRING) CODE = NUM,
  enum Code {
    LINEAR_FCGI_RESPONSE_MAP(LINEAR_FCGI_RESPONSE_ENUM_GEN)
    END_OF_CODE
  };
#undef LINEAR_FCGI_RESPONSE_ENUM_GEN

public:
  static const std::string create(Code code);
  static const std::string create(Code code, const std::string& body,
                                  Http::MimeType type = Http::APP_JAVASCRIPT);

private:
  static const std::string getContentType(Http::MimeType type);
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_RESPONSE_HPP_
