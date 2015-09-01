#include <assert.h>
#include <iostream>
#include <sstream>
#include <uuid/uuid.h>

#include "uv.h"
#include "mod_websocket_base64.h"

#include "config.h"

#ifdef  HAVE_PCRE_H
# include <pcre.h>
#endif /* HAVE_PCRE_H */

#include "fcgi_stdio.h"

#include "linear_fcgi_log.h"
#include "linear_fcgi_server.h"
#include "linear_fcgi_client.h"
#include "linear_fcgi_loop.h"

namespace linear {
namespace fcgi {

using namespace linear::fcgi::log;

Server::Server(const char* path, size_t max_clients, time_t timeout) :
  client_storage_(ClientStorage::getInstance(max_clients)),
  timeout_(timeout) {
  assert((config_ = mod_websocket_config_parse(path)) != NULL);
}

Server::~Server() {
  mod_websocket_config_free(config_);
}

int Server::run() {
  Loop& loop = Loop::getInstance();
  loop.run();

  while (FCGI_Accept() >= 0) {
    Request req = Request::create();
    std::string command = req.getQueries().getCommand();

    switch (req.getMethod()) {
    case Request::GET:
      if (command == "connect") {
        onConnect(req);
      } else if (command == "disconnect") {
        onDisconnect(req);
      } else if (command == "poll") {
        onPoll(req);
      } else {
        onError(req, Response::BAD_REQUEST);
      }
      break;
    case Request::POST:
      onPost(req);
      break;
    default:
      onError(req, Response::METHOD_NOT_ALLOWED);
      break;
    }
  }
  return -1;
}

// This function must be syncronous
void Server::onConnect(const Request& req) {
  std::string id = req.getQueries().getSessionId();
  std::string callback = req.getQueries().getCallback();
  std::string uuid;
  std::ostringstream resp;

  if (id == "") {
    uuid_t uu;
    uuid_generate(uu);
    if (uuid_is_null(uu)) {
      LINEAR_FCGI_LOG(LOG_ERR, "fail to generate uuid\n");
      req.sendResponse(Response::SERVICE_UNAVAILABLE);
      return;
    }
    char uustr[37];
    uuid_unparse(uu, uustr);
    uuid = uustr;

    std::string channel = req.getQueries().getChannel();
    std::string raddr = req.getRemoteAddr();
    std::string origin = req.getOrigin();
    std::stringstream ss;
    int rport;
    ss << req.getRemotePort();
    ss >> rport;
    const mod_websocket_backend_t* backend = getBackend(channel, origin);
    if (backend == NULL) {
      LINEAR_FCGI_LOG(LOG_WARN, "recv connect request from %s:%d: channel == [%s] is invalid\n",
                      raddr.c_str(), rport, channel.c_str());
      onError(req, Response::NOT_FOUND);
      return;
    }
    LINEAR_FCGI_LOG(LOG_INFO, "recv connect request from %s:%d: channel == [%s] (mapped to %s:%d)\n",
                    raddr.c_str(), rport, channel.c_str(), backend->host, backend->port);
    client_storage_.lock();
    Client* client = client_storage_.create(uuid, raddr, rport, backend->origins, timeout_);
    if (client == NULL) {
      client_storage_.unlock();
      req.sendResponse(Response::SERVICE_UNAVAILABLE);
      return;
    }
    bool f = client->connect(backend->host, backend->port);
    if (!f) {
      client_storage_.unlock();
      req.sendResponse(Response::SERVICE_UNAVAILABLE);
      return;
    }
    client_storage_.unlock();
    if (callback == "") {
      resp << "{\"sid\":\"" << uuid << "\"}";
      req.sendResponse(Response::OK, resp.str());
    } else {
      resp << "try { " << callback << "({\"sid\":\"" << uuid << "\"}); } catch(e) {}";
      req.sendResponse(Response::OK, resp.str());
    }
  } else {
    client_storage_.lock();
    Client* client = client_storage_.get(uuid);
    if (client == NULL) {
      client_storage_.unlock();
      req.sendResponse(Response::GONE);
      return;
    }
    client_storage_.unlock();
    if (callback == "") {
      resp << "{\"sid\":\"" << id << "\"}";
      req.sendResponse(Response::OK, resp.str());
    } else {
      resp << "try { " << callback << "({\"sid\":\"" << id << "\"}); } catch(e) {}";
      req.sendResponse(Response::OK, resp.str());
    }
  }
}

void Server::onDisconnect(const Request& req) {
  std::string id = req.getQueries().getSessionId();

  if (id == "") {
    onError(req, Response::BAD_REQUEST);
    return;
  }
  LINEAR_FCGI_LOG(LOG_INFO, "recv disconnect request: %s\n", id.c_str());
  client_storage_.lock();
  Client* client = client_storage_.get(id);
  if (client != NULL) {
    client->disconnect();
  }
  client_storage_.unlock();

  std::string callback = req.getQueries().getCallback();
  std::ostringstream resp;
  if (callback == "") {
    resp << "{\"sid\":\"" << id << "\"}";
    req.sendResponse(Response::OK, resp.str());
  } else {
    resp << "try { " << callback << "({\"sid\":\"" << id << "\"}); } catch(e) {}";
    req.sendResponse(Response::OK, resp.str());
  }
}

void Server::onPoll(const Request& req) {
  std::string id = req.getQueries().getSessionId();

  if (id == "") {
    onError(req, Response::BAD_REQUEST);
    return;
  }

  client_storage_.lock();
  Client* client = client_storage_.get(id);
  if (client == NULL) {
    client_storage_.unlock();
    onError(req, Response::GONE);
    return;
  }

#define DATA_THRESHOLD (16384) // response body MAX_SIZE = 16k
  unsigned char* data = NULL;
  ssize_t siz = 0;
  size_t total_siz = 0;

  do {
    unsigned char* tmp = NULL;
    tcp_buffer_t buf;

    siz = client->read(buf);
    if (siz < 0) {
      client_storage_.unlock();
      onError(req, Response::GONE);
      return;
    }
    if (siz > 0) {
      if (data == NULL) {
        data = static_cast<unsigned char*>(malloc(static_cast<size_t>(siz)));
        if (data == NULL) {
          client_storage_.unlock();
          onError(req, Response::INTERNAL_SERVER_ERROR);
          return;
        }
        memcpy(data, buf.data, static_cast<size_t>(buf.siz));
      } else {
        tmp = static_cast<unsigned char*>(realloc(data, total_siz + static_cast<size_t>(siz)));
        if (tmp == NULL) {
          free(data);
          client_storage_.unlock();
          onError(req, Response::INTERNAL_SERVER_ERROR);
          return;
        }
        data = tmp;
        memcpy(&data[total_siz], buf.data, static_cast<size_t>(buf.siz));
      }
      total_siz = total_siz + static_cast<size_t>(siz);
    }
  } while (siz > 0 && total_siz < DATA_THRESHOLD);
  client_storage_.unlock();
#undef DATA_THRESHOLD

  std::string body("");
  if (total_siz > 0) {
    unsigned char *b64_encoded_data = NULL;
    size_t b64_encoded_siz;
    if (mod_websocket_base64_encode(&b64_encoded_data, &b64_encoded_siz, data, total_siz) < 0) {
      free(data);
      onError(req, Response::INTERNAL_SERVER_ERROR);
      return;
    }
    free(data);
    body = reinterpret_cast<char*>(b64_encoded_data);
    free(b64_encoded_data);
  }

  std::string callback = req.getQueries().getCallback();
  std::ostringstream resp;
  if (callback == "") {
    resp << body;
  } else {
    resp << "try {" << callback << "(\"" << body << "\"); } catch(e) {}";
  }
  req.sendResponse(Response::OK, resp.str());
}

void Server::onPost(const Request& req) {
  std::string id = req.getQueries().getSessionId();

  if (id == "") {
    onError(req, Response::BAD_REQUEST);
    return;
  }
  client_storage_.lock();
  Client* client = client_storage_.get(id);
  if (client == NULL) {
    client_storage_.unlock();
    onError(req, Response::GONE);
    return;
  }
  std::string origin = req.getOrigin();
  if (!isAllowOrigin(origin, client->allowOrigins())) {
    client_storage_.unlock();
    onError(req, Response::BAD_REQUEST);
    return;
  }
  std::string data = req.getQueries().getData();
  if (data.size() > 0) {
    client->write(data);
  }
  client_storage_.unlock();
  req.sendResponse(Response::NO_CONTENT);
}

const mod_websocket_backend_t* Server::getBackend(const std::string& channel, const std::string& origin) {
  if (config_ == NULL || channel.size() == 0) {
    return NULL;
  }
  mod_websocket_resource_t* resource = NULL;
  mod_websocket_backend_t* backend = NULL;

#ifdef HAVE_PCRE_H
  pcre* re = NULL;
  int rc;
  const char* err_str;
  int err_off;
#define N (10)
  int ovec[N * 3];
#endif /* HAVE_PCRE_H */

  for (resource = config_->resources; resource; resource = resource->next) {

#ifdef HAVE_PCRE_H
    LINEAR_FCGI_LOG(LOG_DEBUG, "pcre: key = [%s], channel = [%s]\n", resource->key, channel.c_str());
    re = pcre_compile(resource->key, 0, &err_str, &err_off, NULL);
    rc = pcre_exec(re, NULL, channel.c_str(), static_cast<int>(channel.size()), 0, PCRE_ANCHORED, ovec, N);
    free(re);
    if (rc > 0) {
#undef N
#else
    LINEAR_FCGI_LOG(LOG_DEBUG, "string: key = [%s], channel = [%s]\n", resource->key, channel.c_str());
    if (std::string(resource->key) == channel) {
#endif

      backend = resource->backend;
      if (backend->proto == MOD_WEBSOCKET_BACKEND_PROTOCOL_WEBSOCKET) {
        LINEAR_FCGI_LOG(LOG_DEBUG, "found channel = [%s], but backend is a WebSocket server\n",
                        channel.c_str());
        return NULL;
      }
      if (isAllowOrigin(origin, backend->origins)) {
        LINEAR_FCGI_LOG(LOG_DEBUG, "found channel = [%s]\n", channel.c_str());
        return backend;
      }
      LINEAR_FCGI_LOG(LOG_WARN, "found channel = [%s], but not allowed origin = [%s]\n",
                      channel.c_str(), origin.c_str());
      return NULL;
    }
  }
  LINEAR_FCGI_LOG(LOG_WARN, "not found channel = [%s]\n", channel.c_str());
  return NULL;
}

bool Server::isAllowOrigin(const std::string& origin, const mod_websocket_origin_t* allow_origins) {
  const mod_websocket_origin_t* allow_origin;
#ifdef HAVE_PCRE_H
  pcre* re = NULL;
  int rc;
  const char* err_str;
  int err_off;
#define N (10)
  int ovec[N * 3];
#endif /* HAVE_PCRE_H */

  if (origin.empty() || allow_origins == NULL) {
    LINEAR_FCGI_LOG(LOG_WARN, "origin is empty\n");
    return true;
  }
  for (allow_origin = allow_origins; allow_origin; allow_origin = allow_origin->next) {

#ifdef HAVE_PCRE_H
    re = pcre_compile(allow_origin->origin, 0, &err_str, &err_off, NULL);
    rc = pcre_exec(re, NULL, origin.c_str(), static_cast<int>(origin.size()), 0, PCRE_ANCHORED, ovec, N);
    free(re);
    if (rc > 0) {
#undef N
#else
    if (std::string(allow_origin->origin) == origin) {
#endif

      LINEAR_FCGI_LOG(LOG_DEBUG, "allowed origin = [%s]\n", origin.c_str());
      return true;
    }
  }
  LINEAR_FCGI_LOG(LOG_WARN, "not allowed origin = [%s]\n", origin.c_str());
  return false;
}

} // namespace fcgi
} // namespace linear
