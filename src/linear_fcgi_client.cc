#include <iostream>
#include <assert.h>

#include "mod_websocket_base64.h"
#include "urlendecode.h"

#include "linear_fcgi_log.h"
#include "linear_fcgi_client.h"
#include "linear_fcgi_loop.h"

namespace linear {
namespace fcgi {

using namespace linear::fcgi::log;

Client::Client(const std::string& id, const mod_websocket_origin_t* allow_origins, time_t timeout) :
  id_(id), state_(DISCONNECTED), allow_origins_(allow_origins), timeout_(timeout){
  assert(pthread_mutex_init(&state_mutex_, NULL) == 0);
  assert(pthread_cond_init(&state_cond_, NULL) == 0);
}

Client::~Client() {
  pthread_mutex_destroy(&state_mutex_);
  pthread_cond_destroy(&state_cond_);
}

bool Client::connect(const char* host, int port) {
  lock();
  if (state_ == CONNECTING || state_ == CONNECTED) {
    unlock();
    return true;
  }
  if (state_ == DISCONNECTING) {
    unlock();
    return false;
  }
  host_ = host;
  port_ = port;
  request_t request(CONNECT, this);
  Loop& loop = Loop::getInstance();
  loop.execute(request);
  state_ = CONNECTING;

 again:
  struct timespec timeout;
  timeout.tv_sec = time(NULL) + 1;
  timeout.tv_nsec = 0;
  int err = pthread_cond_timedwait(&state_cond_, &state_mutex_, &timeout);
  if (state_ == CONNECTED) {
    unlock();
    return true;
  } else if (state_ == FAIL_CONNECT) {
    LINEAR_FCGI_LOG(LOG_WARN, "fail to connect: %s\n", id_.c_str());
    state_ = FAIL_CONNECT;
    unlock();
    return false;
  } else if (err) {
    LINEAR_FCGI_LOG(LOG_WARN, "fail to connect: %s\n", id_.c_str());
    unlock();
    disconnect();
    return false;
  } else {
    LINEAR_FCGI_DEBUG(LOG_DEBUG, "got SIGNAL\n");
    goto again;
  }
}

void Client::disconnect() {
  lock();
  if (state_ == DISCONNECTING || state_ == DISCONNECTED || state_ == FAIL_CONNECT) {
    unlock();
    return;
  }
  state_ = DISCONNECTING;
  unlock();
  request_t request(DISCONNECT, this);
  Loop& loop = Loop::getInstance();
  loop.execute(request);
}

ssize_t Client::read(tcp_buffer_t& buf) {
  lock();
  if (state_ != CONNECTING && state_ != CONNECTED) {
    unlock();
    return -1;
  }
  unlock();
  last_access_ = time(NULL);
  if (!read_queue_.consume(buf)) {
    return 0;
  }
  return buf.siz;
}

ssize_t Client::write(const std::string& data) {
  lock();
  if (state_ == DISCONNECTING || state_ == DISCONNECTED) {
    unlock();
    return -1;
  }
  unlock();
  write_queue_.produce(data);
  request_t request(WRITE, this);
  Loop& loop = Loop::getInstance();
  loop.execute(request);
  return static_cast<ssize_t>(data.size());
}

// called from loop thread
void Client::_connect() {
  lock();
  if (uv_tcp_init(uv_default_loop(), &handle_) != 0) {
    state_ = FAIL_CONNECT;
    pthread_cond_signal(&state_cond_);
    unlock();
    return;
  }
  uv_connect_t* connect_req = static_cast<uv_connect_t*>(malloc(sizeof(uv_connect_t)));
  if (connect_req == NULL) {
    state_ = FAIL_CONNECT;
    pthread_cond_signal(&state_cond_);
    unlock();
    return;
  }
  connect_req->data = this;
  LINEAR_FCGI_LOG(LOG_INFO, "connecting: %s\n", id_.c_str());

  struct addrinfo hints;
  struct addrinfo *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG;
  if (getaddrinfo(host_.c_str(), NULL, &hints, &res) != 0) {
    LINEAR_FCGI_DEBUG(LOG_ERR, "fail getaddrinfo: %s\n", host_.c_str());
    state_ = FAIL_CONNECT;
    pthread_cond_signal(&state_cond_);
    unlock();
    return;
  }

  int ret = -1;
  if (res->ai_family == AF_INET) {
    struct sockaddr_in addr4;
    uv_ip4_addr(host_.c_str(), port_, &addr4);
    ret = uv_tcp_connect(connect_req, &handle_, reinterpret_cast<const struct sockaddr*>(&addr4), Loop::onConnect);
  } else if (res->ai_family == AF_INET6) {
    struct sockaddr_in6 addr6;
    uv_ip6_addr(host_.c_str(), port_, &addr6);
    ret = uv_tcp_connect(connect_req, &handle_, reinterpret_cast<const struct sockaddr*>(&addr6), Loop::onConnect);
  }
  freeaddrinfo(res);
  if (ret != 0) {
    free(connect_req);
    state_ = FAIL_CONNECT;
    pthread_cond_signal(&state_cond_);
  }
  unlock();
}

void Client::_write() {
  std::string data;

  while (write_queue_.consume(data)) {
    char* urldecoded = url_decode(data.c_str());
    if (urldecoded == NULL) {
      return;
    }
    unsigned char* b64_decoded_data = NULL;
    size_t b64_decoded_siz;
    if (mod_websocket_base64_decode(&b64_decoded_data, &b64_decoded_siz,
                                    reinterpret_cast<unsigned char *>(urldecoded)) < 0) {
      free(urldecoded);
      return;
    }
    free(urldecoded);
    uv_buf_t buf;
    buf.base = reinterpret_cast<char*>(b64_decoded_data);
    buf.len = b64_decoded_siz;
    uv_write_t* write_req = static_cast<uv_write_t*>(malloc(sizeof(uv_write_t)));
    if (write_req == NULL) {
      free(buf.base);
      return;
    }
    write_req->data = buf.base;
    int r = uv_write(write_req, reinterpret_cast<uv_stream_t*>(&handle_),
                     &buf, 1, Loop::onWrite);
    if (r < 0) {
      free(write_req);
      free(buf.base);
    }
  }
}

void Client::_disconnect() {
  uv_handle_t* disconnect_req = reinterpret_cast<uv_handle_t*>(&handle_);
  disconnect_req->data = this;
  LINEAR_FCGI_LOG(LOG_INFO, "disconnecting: %s\n", id_.c_str());
  uv_close(disconnect_req, Loop::onDisconnect);
}

void Client::onConnect(int status) {
  lock();
  if (status != 0 || state_ != CONNECTING) {
    state_ = FAIL_CONNECT;
    pthread_cond_signal(&state_cond_);
    unlock();
    return;
  }
  LINEAR_FCGI_LOG(LOG_INFO, "connected: %s\n", id_.c_str());
  state_ = CONNECTED;
  pthread_cond_signal(&state_cond_);
  unlock();
  last_access_ = time(NULL);
  uv_stream_t* stream = reinterpret_cast<uv_stream_t*>(&handle_);
  stream->data = this;
  uv_read_start(stream, Loop::onAlloc, Loop::onRead);
  return;
}

void Client::onDisconnect() {
  LINEAR_FCGI_LOG(LOG_INFO, "disconnected: %s\n", id_.c_str());
}

void Client::onRead(const char* data, ssize_t siz) {
  lock();
  if (state_ != CONNECTED) {
    unlock();
    return;
  }
  if (siz < 0) {
    LINEAR_FCGI_LOG(LOG_WARN, "maybe down backend (%s:%d)\n", host_.c_str(), port_);
    state_ = DISCONNECTING;
    _disconnect();
    unlock();
    return;
  }
  unlock();
  if (siz > 0) {
    char* clone = static_cast<char*>(malloc(static_cast<size_t>(siz)));
    if (clone == NULL) {
      return;
    }
    memcpy(clone, data, static_cast<size_t>(siz));
    tcp_buffer_t buf(clone, siz);
    read_queue_.produce(buf);
    return;
  }
}

void Client::onTimer() {
  time_t past = time(NULL) - last_access_;
  lock();
  if ((past >= timeout_ && (state_ == CONNECTED || state_ == CONNECTING)) ||
      state_ == FAIL_CONNECT) {
    if (state_ == FAIL_CONNECT) {
      LINEAR_FCGI_LOG(LOG_DEBUG, "GC for fail to connect: %s\n", id_.c_str());
    } else {
      LINEAR_FCGI_LOG(LOG_INFO, "timeout %d sec: %s\n", static_cast<int>(past), id_.c_str());
    }
    state_ = DISCONNECTING;
    _disconnect();
  }
  unlock();
}

// private
void Client::lock() {
  assert(pthread_mutex_lock(&state_mutex_) == 0);
}

void Client::unlock() {
  assert(pthread_mutex_unlock(&state_mutex_) == 0);
}

} // namespace fcgi
} // namespace linear
