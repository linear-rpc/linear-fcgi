#ifndef _LINEAR_FCGI_LOOP_H_
#define _LINEAR_FCGI_LOOP_H_

#include <string>

#include "uv.h"

#include "linear_fcgi_client.h"
#include "linear_fcgi_client_storage.h"
#include "linear_fcgi_queue.h"

namespace linear {
namespace fcgi {

enum RequestType {
  UNDEFINED,
  CONNECT,
  DISCONNECT,
  WRITE
};

struct request_t {
  request_t() : type(UNDEFINED), client(NULL) {}
  request_t(RequestType t, Client* c) : type(t), client(c) {}
  RequestType type;
  Client* client;
};

class Loop {
public:
  static Loop& getInstance();

public:
  int run();
  bool execute(const request_t& request);

public:
  static void mainloop(void* data);
  static void onAsync(uv_async_t* async);
  static void onConnect(uv_connect_t* connect_req, int status);
  static void onDisconnect(uv_handle_t* disconnect_req);
  static void onAlloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
  static void onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
  static void onWrite(uv_write_t* write_req, int status);
  static void onTimer(uv_timer_t* timer);

private:
  Loop() {}
  Loop(const Loop& rhs);
  Loop& operator=(const Loop& rhs);
  ~Loop() {}

private:
  uv_thread_t tid_;
  uv_async_t async_;
  uv_timer_t timer_;
  Queue<request_t> request_queue_;
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_LOOP_H_
