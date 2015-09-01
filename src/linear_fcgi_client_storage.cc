#include <assert.h>
#include <sys/syscall.h>

#include "linear_fcgi_log.h"
#include "linear_fcgi_client_storage.h"
#include "linear_fcgi_client.h"

namespace linear {
namespace fcgi {

using namespace linear::fcgi::log;

ClientStorage& ClientStorage::getInstance(size_t max_clients) {
  static ClientStorage instance(max_clients);
  return instance;
}

Client* ClientStorage::create(const std::string& id,
                              const std::string& raddr, int rport,
                              const mod_websocket_origin_t* allow_origins,
                              time_t timeout) {
  std::map<std::string, Client*>::iterator it;
  Client* c = NULL;

  if (client_map_.size() >= max_clients_) {
    return NULL;
  }
  it = client_map_.find(id);
  if (it != client_map_.end()) {
    return NULL;
  }
  try {
    c = new Client(id, allow_origins, timeout);
  } catch (const std::bad_alloc& e) {
    return NULL;
  }
  client_map_.insert(std::make_pair(id, c));
  LINEAR_FCGI_LOG(LOG_DEBUG, "created: %s for %s:%d\n", id.c_str(), raddr.c_str(), rport);
  return c;
}

void ClientStorage::destroy(const std::string& id) {
  std::map<std::string, Client*>::iterator it;
  Client* c = NULL;

  it = client_map_.find(id);
  if (it != client_map_.end()) {
    c = it->second;
    LINEAR_FCGI_LOG(LOG_DEBUG, "destroyed: %s\n", id.c_str());
    client_map_.erase(it);
    delete c;
  }
}

Client* ClientStorage::get(const std::string& id) {
  std::map<std::string, Client*>::iterator it;

  it = client_map_.find(id);
  if (it != client_map_.end()) {
    return it->second;
  }
  return NULL;
}

void ClientStorage::onTimer() {
  std::map<std::string, Client*>::iterator it;

  it = client_map_.begin();
  while (it != client_map_.end()) {
    (it->second)->onTimer();
    it++;
  }
}

void ClientStorage::lock() {
  assert(pthread_mutex_lock(&lock_) == 0);
}

void ClientStorage::unlock() {
  assert(pthread_mutex_unlock(&lock_) == 0);
}

ClientStorage::ClientStorage(size_t max_clients) : max_clients_(max_clients) {
  assert(pthread_mutex_init(&lock_, NULL) == 0);
}

ClientStorage::~ClientStorage() {
  pthread_mutex_destroy(&lock_);
}

} // namespace fcgi
} // namespace linear

