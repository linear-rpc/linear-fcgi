#include <sys/time.h>

#include <sstream>
#include <iomanip>

#include "linear_fcgi_log.h"

namespace linear {

namespace fcgi {

static Log g_log;

namespace log {

static Level g_level;

/* functions */
void SetLevel(Level level) {
  g_level = level;
}

void Enable(const std::string& fname) {
  g_log.Enable(fname);
}

void Print(linear::fcgi::log::Level level, const char* file, int line, const char* func, const char* format, ...) {
  va_list args;
  char buffer[LOG_BUFSIZ];

  if (g_level < level) {
    return;
  }

  va_start(args, format);
  vsnprintf(buffer, LOG_BUFSIZ, format, args);
  va_end(args);

  g_log.Write(level, file, line, func, buffer);
}

}  // namespace log

/* Log class methods */
Log::Log() {
  uv_mutex_init(&mutex_);
}

Log::~Log() {
  Disable();
  uv_mutex_destroy(&mutex_);
}

void Log::Enable(const std::string& fname) {
  uv_mutex_lock(&mutex_);
  fp_ = fopen(fname.c_str(), "a");
  uv_mutex_unlock(&mutex_);
}

void Log::Disable() {
  uv_mutex_lock(&mutex_);
  if (fp_ != NULL) {
    fclose(fp_);
    fp_ = NULL;
  }
  uv_mutex_unlock(&mutex_);
}

void Log::Write(linear::fcgi::log::Level level, const char* file, int line, const char* func, const char* message) {
  std::string fname(file);

#define SEPARATOR '/'
  std::string::size_type n = fname.find_last_of(SEPARATOR);
  (void) n;
#undef SEPARATOR

  if (fp_ == NULL) {
    return;
  }

  uv_mutex_lock(&mutex_);
  const char* strptr = "";
#define LINEAR_FCGI_LOG_LEVEL_CASE_GEN(IDENT, NUM, LONG_STR, SHORT_STR) \
  case linear::fcgi::log::IDENT:                                        \
    strptr = SHORT_STR;                                                 \
    break;
  switch(level) {
    LINEAR_FCGI_LOG_LEVEL_MAP(LINEAR_FCGI_LOG_LEVEL_CASE_GEN)
  default:
      break;
  }
#undef LINEAR_FCGI_LOG_LEVEL_CASE_GEN

  fprintf(fp_, "%s: ", GetDateTime().c_str());
  fprintf(fp_, "[%s] ", strptr);

#ifdef _LINEAR_FCGI_LOG_DEBUG_
  fprintf(fp_, "(%s:%d) ",
          (n == std::string::npos) ? fname.c_str() : fname.substr(n + 1).c_str(),
          line);
#endif

  fprintf(fp_, "%s", message);
  fflush(fp_);
  uv_mutex_unlock(&mutex_);
}

std::string Log::GetDateTime() {
  char datetime_str[32];

#ifdef _WIN32
  SYSTEMTIME st;
  GetLocalTime(&st);
  _snprintf_s(datetime_str, sizeof(datetime_str), _TRUNCATE,
              "%d-%02d-%02d %02d:%02d:%02d.%03d",
              st.wYear, st.wMonth, st.wDay,
              st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
  struct tm ts;
  struct timeval now;
  struct tm* ret = NULL;
  if (gettimeofday(&now, 0) == 0) {
    ret = localtime_r(&now.tv_sec, &ts);
  }
  if (ret == NULL) {
    snprintf(datetime_str, sizeof(datetime_str), "ERR: fail to get date");
  } else {
    snprintf(datetime_str, sizeof(datetime_str),
             "%d-%02d-%02d %02d:%02d:%02d.%03d",
             ts.tm_year + 1900, ts.tm_mon + 1, ts.tm_mday,
             ts.tm_hour, ts.tm_min, ts.tm_sec,
             static_cast<int>(now.tv_usec / 1000));
  }
#endif

  return std::string(datetime_str);
}

}  // namespace fcgi

}  // namespace linear
