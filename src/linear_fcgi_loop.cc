#include <assert.h>

#include "linear_fcgi_loop.h"

namespace linear {
namespace fcgi {

Loop& Loop::getInstance() {
  static Loop instance;
  return instance;
}

int Loop::run() {
  int ret = uv_async_init(uv_default_loop(), &async_, Loop::onAsync);
  if (ret != 0) {
    assert(false);
  }
  async_.data = &request_queue_;
  ret = uv_timer_init(uv_default_loop(), &timer_);
  if (ret != 0) {
    assert(false);
  }
  timer_.data = NULL;
  uv_timer_start(&timer_, Loop::onTimer, 1000, 1000);
  return uv_thread_create(&tid_, mainloop, NULL);
}

bool Loop::execute(const request_t& request) {
  request_queue_.produce(request);
  return (uv_async_send(&async_) == 0);
}

// child thread
void Loop::mainloop(void* data) {
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

void Loop::onAsync(uv_async_t* async) {
  Queue<request_t>* request_queue = reinterpret_cast<Queue<request_t>* >(async->data);
  request_t request;

  while (request_queue->consume(request)) {
    Client* client = request.client;
    switch(request.type) {
    case CONNECT:
      client->_connect();
      break;
    case DISCONNECT:
      client->_disconnect();
      break;
    case WRITE:
      client->_write();
      break;
    default:
      break;
    }
  }
}

void Loop::onConnect(uv_connect_t* connect_req, int status) {
  Client* client = reinterpret_cast<Client*>(connect_req->data);
  client->onConnect(status);
  free(connect_req);
}

void Loop::onDisconnect(uv_handle_t* disconnect_req) {
  Client* client = reinterpret_cast<Client*>(disconnect_req->data);
  client->onDisconnect();

  ClientStorage& client_storage = ClientStorage::getInstance();
  client_storage.lock();
  client_storage.destroy(client->id());
  client_storage.unlock();
}

void Loop::onAlloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
  buf->base = static_cast<char*>(malloc(suggested_size));
  buf->len = static_cast<unsigned int>(suggested_size);
}

void Loop::onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  Client* client = reinterpret_cast<Client*>(stream->data);
  client->onRead(buf->base, nread);
  free(buf->base);
}

void Loop::onWrite(uv_write_t* write_req, int status) {
  char* data = reinterpret_cast<char*>(write_req->data);
  free(data);
  free(write_req);
}

void Loop::onTimer(uv_timer_t* timer) {
  ClientStorage& client_storage = ClientStorage::getInstance();
  client_storage.lock();
  client_storage.onTimer();
  client_storage.unlock();
}

} // namespace fcgi
} // namespace linear
