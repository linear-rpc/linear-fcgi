#ifndef _LINEAR_FCGI_CLIENT_STORAGE_H_
#define _LINEAR_FCGI_CLIENT_STORAGE_H_

#include <time.h>
#include <pthread.h>

#include <string>
#include <map>

#include "mod_websocket_config.h"

namespace linear {
namespace fcgi {

#define ID_NONE ""
#define DEFAULT_MAX_CLIENTS (8)
#define DEFAULT_TIMEOUT (30)

class Client;

class ClientStorage {
public:
  static ClientStorage& getInstance(size_t max_clients = DEFAULT_MAX_CLIENTS);

public:
  Client* create(const std::string& id, const std::string& raddr, int rport,
                 const mod_websocket_origin_t* allow_origins, time_t timeout = DEFAULT_TIMEOUT);
  void destroy(const std::string& id);
  Client* get(const std::string& id);
  void onTimer();
  void lock();
  void unlock();

private:
  ClientStorage(size_t max_clients);
  ClientStorage();
  ClientStorage(const ClientStorage& rhs);
  ClientStorage& operator=(const ClientStorage& rhs);
  ~ClientStorage();

private:
  size_t max_clients_;
  pthread_mutex_t lock_;
  std::map<std::string, Client*> client_map_;
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_CLIENT_STORAGE_HPP_
