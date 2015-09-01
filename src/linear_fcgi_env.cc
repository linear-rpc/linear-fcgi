#include <stdlib.h>
#include <string>

#include "linear_fcgi_env.h"

namespace linear {
namespace fcgi {

#define LINEAR_FCGI_ENV_CASE_GEN(IDENT, STRING) \
  case IDENT: \
    cptr = getenv(STRING); \
    break;

const std::string Env::get(Env::Ident ident) {
  const char* cptr = NULL;
  switch (ident) {
    LINEAR_FCGI_ENV_MAP(LINEAR_FCGI_ENV_CASE_GEN);
  default:
    break;
  }
  return std::string(cptr ? cptr : "");
}

} // namespace fcgi
} // namespace linear
