#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "linear_fcgi_log.h"
#include "linear_fcgi_server.h"

using namespace linear::fcgi;
using namespace linear::fcgi::log;

Level setup_log() {
  const char* fname = getenv("LINEAR_FCGI_LOG_FILE");
  Level level = LOG_ERR;

  if (fname != NULL) {
    level = LOG_ERR;
    const char* str_level = getenv("LINEAR_FCGI_LOG_LEVEL");
    if (str_level != NULL) {
      level = static_cast<Level>(strtol(str_level, NULL, 10));
    }
    linear::fcgi::log::SetLevel(static_cast<Level>(level));
    linear::fcgi::log::Enable(fname);
  }
  return level;
}

int main() {
  Level log_level = setup_log();
  const char* conf_fname = getenv("LINEAR_FCGI_WEBSOCKET_CONFIG_PATH");
  if (conf_fname == NULL) {
    LINEAR_FCGI_LOG(LOG_ERR, "LINEAR_FCGI_WEBSOCKET_CONFIG_PATH is required\n");
    return -1;
  }

#define LINEAR_FCGI_LOG_LEVEL_CASE_GEN(IDENT, NUM, LONG_STR, SHORT_STR)     \
  case IDENT: \
    cptr = LONG_STR; \
    break;

  const char* cptr = "";
  switch(log_level) {
    LINEAR_FCGI_LOG_LEVEL_MAP(LINEAR_FCGI_LOG_LEVEL_CASE_GEN)
  default:
      break;
  }
  LINEAR_FCGI_LOG(LOG_ERR, "log file: %s, level: %s\n", conf_fname, cptr);

#undef LINEAR_FCGI_LOG_GET_LEVEL_STRING

  size_t max_clients = DEFAULT_MAX_CLIENTS;
  const char* str_max_clients = getenv("LINEAR_FCGI_MAX_CLIENTS");
  if (str_max_clients != NULL) {
    max_clients = static_cast<size_t>(strtol(str_max_clients, NULL, 10));
  }
  time_t timeout = DEFAULT_TIMEOUT;
  const char* str_timeout = getenv("LINEAR_FCGI_TIMEOUT");
  if (str_timeout != NULL) {
    timeout = static_cast<time_t>(strtol(str_timeout, NULL, 10));
  }
  LINEAR_FCGI_DEBUG(LOG_ERR, "max_clients: %u, timeout: %d sec\n", max_clients, timeout);
  Server server(conf_fname, max_clients, timeout);
  return server.run();
}
