#include <new>
#include <iostream>
#include <sstream>

#include "linear_fcgi_queries.h"

namespace linear {
namespace fcgi {

Queries::Queries(const std::string& data) {
  queries_.clear();
  parse(data);
}

const std::string& Queries::getSessionId() const {
  static std::string err("");
  std::map<std::string, std::string>::const_iterator it = queries_.find(QueryKeys::SESSION_ID);
  if (it != queries_.end()) {
    return (*it).second;
  }
  return err;
}

const std::string& Queries::getChannel() const {
  static std::string err("");
  std::map<std::string, std::string>::const_iterator it = queries_.find(QueryKeys::CHANNEL);
  if (it != queries_.end()) {
    return (*it).second;
  }
  return err;
}

const std::string& Queries::getCommand() const {
  static std::string err("");
  std::map<std::string, std::string>::const_iterator it = queries_.find(QueryKeys::COMMAND);
  if (it != queries_.end()) {
    return (*it).second;
  }
  return err;
}

const std::string& Queries::getData() const {
  static std::string err("");
  std::map<std::string, std::string>::const_iterator it = queries_.find(QueryKeys::DATA);
  if (it != queries_.end()) {
    return (*it).second;
  }
  return err;
}

const std::string& Queries::getCallback() const {
  static std::string err("");
  std::map<std::string, std::string>::const_iterator it = queries_.find(QueryKeys::CALLBACK);
  if (it != queries_.end()) {
    return (*it).second;
  }
  return err;
}

void Queries::parse(const std::string& data) {
  std::string word;
  std::vector<std::string> words;
  std::vector<std::string>::iterator it;
  std::string key, val;

  words = split(data);
  for (it = words.begin(); it != words.end(); ++it) {
    word = *it;
    const std::string::size_type pos = word.find('=');
    if (pos != std::string::npos) {
      key = word.substr(0, pos);
      val = word.substr(pos + 1);
      queries_.insert(make_pair(key, val));
    }
  }
}

std::vector<std::string> Queries::split(const std::string& data) {
  std::istringstream is(data);
  std::string tmp;
  std::vector<std::string> result;

  while (getline(is, tmp, '&')) {
    result.push_back(tmp);
  }
  return result;
}

} // namespace fcgi
} // namespace linear
