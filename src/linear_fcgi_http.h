#ifndef _LINEAR_FCGI_HTTP_H_
#define _LINEAR_FCGI_HTTP_H_

namespace linear {
namespace fcgi {

#define LINEAR_FCGI_MIME_TYPE_MAP(GEN)                                  \
  GEN(TEXT_PLAIN, "text/plain")                                         \
  GEN(TEXT_JAVASCRIPT, "text/javascript")                               \
  GEN(APP_JSON, "application/json")                                     \
  GEN(APP_JAVASCRIPT, "application/javascript")                         \
  GEN(X_WWW_FORM_URLENCODED, "application/x-www-form-urlencoded")

class Http {
public:
#define LINEAR_FCGI_MIME_TYPE_ENUM_GEN(IDENT, STRING) IDENT,
  enum MimeType {
    LINEAR_FCGI_MIME_TYPE_MAP(LINEAR_FCGI_MIME_TYPE_ENUM_GEN)
    UNKNOWN
  };
#undef LINEAR_FCGI_MIME_TYPE_ENUM_GEN
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_HTTP_H_

