#ifndef	LINEAR_FCGI_LOG_H_
#define	LINEAR_FCGI_LOG_H_

#include "uv.h"

#include <string>

#include "fcgi_stdio.h"

#define LOG_BUFSIZ (16384)

#define LINEAR_FCGI_LOG(level, format, ...)                             \
  do {                                                                  \
    linear::fcgi::log::Print(level, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, ##__VA_ARGS__); \
  } while(0)

#ifdef _LINEAR_FCGI_DEBUG_
#define LINEAR_FCGI_DEBUG(level, format, ...)                                  \
  do {                                                                  \
    linear::fcgi::log::Print(level, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, ##__VA_ARGS__); \
  } while(0)
#else
# define LINEAR_FCGI_DEBUG(level, format, ...)
#endif

#define LINEAR_FCGI_LOG_LEVEL_MAP(GEN)                 \
  GEN(LOG_OFF,   0, "",        ""   )             \
  GEN(LOG_ERR,   1, "ERROR",   "ERR")             \
  GEN(LOG_WARN,  2, "WARNING", "WRN")             \
  GEN(LOG_INFO,  3, "INFO",    "INF")             \
  GEN(LOG_DEBUG, 4, "DEBUG",   "DBG")

#define LINEAR_FCGI_LOG_LEVEL_GEN(IDENT, NUM, LONG_STRING, SHORT_STRING) IDENT = NUM,

namespace linear {

namespace fcgi {

namespace log {

enum Level {
  LINEAR_FCGI_LOG_LEVEL_MAP(LINEAR_FCGI_LOG_LEVEL_GEN)
};

void Enable(const std::string& fname);
void SetLevel(linear::fcgi::log::Level level);
void Print(linear::fcgi::log::Level level, const char* file, int line, const char* func, const char* format, ...);

} // namespace log

class Log {
 public:
  Log();
  ~Log();
  void Enable(const std::string& fname);
  void Disable();
  void Write(linear::fcgi::log::Level level, const char* fname, int line, const char* func, const char* message);

 private:
  Log(const Log& rhs);
  Log& operator=(const Log& rhs);
  std::string GetDateTime();

  FILE* fp_;
  uv_mutex_t mutex_;
};

} // namespace fcgi

} // namespace linear

#endif	// LINEAR_FCGI_LOG_H_
