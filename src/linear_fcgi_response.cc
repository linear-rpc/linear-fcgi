#include <sstream>

#include "linear_fcgi_response.h"

namespace linear {
namespace fcgi {

const std::string Response::create(Code code) {
  std::ostringstream os;
  os << "Status: " << code << "\r\n\r\n";
  return os.str();
}

const std::string Response::create(Code code, const std::string& body, Http::MimeType type) {
  std::ostringstream os;
  os << "Status: " << code << "\r\n";
  os << "Content-Type: " << Response::getContentType(type) << "; charset=utf-8\r\n";
  os << "X-Content-Type-Options: nosniff" << "\r\n";
  os << "Content-Length: " << body.size() << "\r\n\r\n";
  os << body;
  return os.str();
}

const std::string Response::getContentType(Http::MimeType type) {
#define LINEAR_FCGI_MIME_TYPE_ENUM_DECL_GEN(IDENT, STRING)      \
  const Http::MimeType ENUM_##IDENT = Http::IDENT;
#define LINEAR_FCGI_RESPONSE_CONTENT_TYPE_GEN(TYPE, STRING)     \
  case ENUM_##TYPE:                                             \
    cptr = STRING;                                              \
    break;

  LINEAR_FCGI_MIME_TYPE_MAP(LINEAR_FCGI_MIME_TYPE_ENUM_DECL_GEN);
  const char* cptr = NULL;
  switch (type) {
    LINEAR_FCGI_MIME_TYPE_MAP(LINEAR_FCGI_RESPONSE_CONTENT_TYPE_GEN);
  default:
    break;
  }
  return std::string(cptr ? cptr : "application/x-www-form-urlencoded");

#undef LINEAR_FCGI_RESPONSE_CONTENT_TYPE_GEN
#undef LINEAR_FCGI_MIME_TYPE_ENUM_DECL_GEN
}

} // namespace fcgi
} // namespace linear
