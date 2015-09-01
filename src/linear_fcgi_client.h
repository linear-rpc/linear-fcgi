#ifndef _LINEAR_FCGI_CLIENT_H_
#define _LINEAR_FCGI_CLIENT_H_

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <string>

#include "uv.h"
#include "mod_websocket_config.h"

#include "linear_fcgi_queue.h"

namespace linear {
namespace fcgi {

struct tcp_buffer_t {
  tcp_buffer_t() : data(NULL), siz(0) {}
  tcp_buffer_t(const char* d, ssize_t s) : data(const_cast<char*>(d)), siz(s) {}
  tcp_buffer_t(const tcp_buffer_t& rhs) {
    if (rhs.siz > 0) {
      data = static_cast<char*>(malloc(static_cast<size_t>(rhs.siz)));
      memcpy(data, rhs.data, static_cast<size_t>(rhs.siz));
      siz = rhs.siz;
    } else {
      data = NULL;
      siz = 0;
    }
  }
  tcp_buffer_t& operator=(const tcp_buffer_t& rhs) {
    if (rhs.siz > 0) {
      data = static_cast<char*>(malloc(static_cast<size_t>(rhs.siz)));
      memcpy(data, rhs.data, static_cast<size_t>(rhs.siz));
      siz = rhs.siz;
    } else {
      data = NULL;
      siz = 0;
    }
    return *this;
  }
  ~tcp_buffer_t() {
    if (data) {
      free(data);
      data = NULL;
    }
  }
  char* data;
  ssize_t siz;
};

class Client {
public:
  enum State {
    CONNECTING,
    FAIL_CONNECT,
    CONNECTED,
    DISCONNECTING,
    DISCONNECTED
  };

public:
  Client(const std::string& id, const mod_websocket_origin_t* allow_origins, time_t timeout);
  ~Client();

  const std::string& id() { return id_; }
  const mod_websocket_origin_t* allowOrigins() { return allow_origins_; }
  bool connect(const char* host, int port);
  void disconnect();
  ssize_t read(tcp_buffer_t& buf);
  ssize_t write(const std::string& buf);

  void _connect();
  void _disconnect();
  void _write();
  void onConnect(int status);
  void onDisconnect();
  void onRead(const char* data, ssize_t siz);
  void onTimer();

private:
  std::string id_;
  std::string host_;
  int port_;
  State state_;
  const mod_websocket_origin_t* allow_origins_;
  time_t last_access_, timeout_;
  pthread_mutex_t state_mutex_;
  pthread_cond_t state_cond_;
  uv_tcp_t handle_;
  Queue<tcp_buffer_t> read_queue_;
  Queue<std::string> write_queue_;

private:
  void lock();
  void unlock();
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_CLIENT_H_
