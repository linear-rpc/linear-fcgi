#ifndef _LINEAR_FCGI_SERVER_H_
#define _LINEAR_FCGI_SERVER_H_

#include "mod_websocket_config.h"

#include "linear_fcgi_request.h"
#include "linear_fcgi_client_storage.h"

namespace linear {
namespace fcgi {

class Server {
public:
  Server(const char* path, size_t max_clients, time_t timeout);
  ~Server();

public:
  int run();
  static void sighdl(int type);

private:
  void onConnect(const Request& req);
  void onDisconnect(const Request& req);
  void onPoll(const Request& req);
  void onPost(const Request& req);
  inline void onError(const Request& req,
                      Response::Code code = Response::INTERNAL_SERVER_ERROR) {
      req.sendResponse(code);
  }
  const mod_websocket_backend_t* getBackend(const std::string& channel,
                                            const std::string& origin);
  bool isAllowOrigin(const std::string& origin, const mod_websocket_origin_t* allow_origins);

private:
  ClientStorage& client_storage_;
  mod_websocket_config_t* config_;
  time_t timeout_;
};

} // namespace fcgi
} // namespace linear

#endif // _LINEAR_FCGI_SERVER_HPP_
