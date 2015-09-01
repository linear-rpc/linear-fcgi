#ifndef _LINEAR_FCGI_QUERIES_H_
#define _LINEAR_FCGI_QUERIES_H_

#include <string>
#include <vector>
#include <map>

namespace linear {
namespace fcgi {

#define LINEAR_FCGI_QUERY_KEYS_MAP(GEN)  \
  GEN(SESSION_ID, "sid")                 \
  GEN(CHANNEL, "ch")                     \
  GEN(COMMAND, "c")                      \
  GEN(DATA, "data")                      \
  GEN(CALLBACK, "f")

namespace QueryKeys {

#define LINEAR_FCGI_QUERY_KEYS_GEN(IDENT, STRING) static const char IDENT[] = STRING;
  LINEAR_FCGI_QUERY_KEYS_MAP(LINEAR_FCGI_QUERY_KEYS_GEN)
#undef LINEAR_FCGI_QUERIES_GEN

} // namespace QueryKeys

class Queries {
public:
  Queries(const std::string& data);
  ~Queries() {}

public:
  const std::string& getSessionId() const;
  const std::string& getChannel() const;
  const std::string& getCommand() const;
  const std::string& getData() const;
  const std::string& getCallback() const;

private:
  void parse(const std::string& data);
  std::vector<std::string> split(const std::string& data);

private:
  std::map<std::string, std::string> queries_;
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_QUERIES_HPP_
