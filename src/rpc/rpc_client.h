/*
 * rpc_client.h
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <sofa/pbrpc/pbrpc.h>
#include <assert.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <mutex.h>
#include <thread_pool.h>
#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::DEBUG;
using ::baidu::common::WARNING;

namespace el {
 
class RpcClient {
public:
  RpcClient() {
    sofa::pbrpc::RpcClientOptions options;
    options.max_pending_buffer_size = 128;
    _rpc_client = new sofa::pbrpc::RpcClient(options);
  }
  ~RpcClient() {
    _rpc_client->Shutdown();
    delete _rpc_client;
  }

  template <class T>
  bool GetStub(const std::string server, T** stub) {
    ::baidu::common::MutexLock lock(&_host_map_lock);
    sofa::pbrpc::RpcChannel* channel = NULL;
    HostMap::iterator it = _host_map.find(server);
    if (it != _host_map.end()) {
      channel = it->second;
    } else {
      sofa::pbrpc::RpcChannelOptions channel_options;
      channel = new sofa::pbrpc::RpcChannel(_rpc_client, server, channel_options);
       _host_map[server] = channel;
    }
    *stub = new T(channel);
    return true;
  }

  template <class Stub, class Request, class Response, class Callback>
  bool SendRequest(Stub* stub, void(Stub::*func)(
                    google::protobuf::RpcController*,
                    const Request*, Response*, Callback*),
                    const Request* request, Response* response,
                    int32_t rpc_timeout, int retry_times) {
    sofa::pbrpc::RpcController controller;
    controller.SetTimeout(rpc_timeout * 1000L);
    for (int32_t retry = 0; retry < retry_times; ++retry) {
      (stub->*func)(&controller, request, response, NULL);
      if (controller.Failed()) {
        if (retry < retry_times - 1) {
          LOG(DEBUG, "Send failed, retry ...\n");
          usleep(1000000);
        } else {
          LOG(WARNING, "SendRequest fail: %s\n", controller.ErrorText().c_str());
        }
      } else {
        return true;
      }
      controller.Reset();
    }
    return false;
  }
  
  template <class Stub, class Request, class Response, class Callback>
  void AsyncRequest(Stub* stub, void(Stub::*func)(
                    google::protobuf::RpcController*,
                    const Request*, Response*, Callback*),
                    const Request* request, Response* response,
                    boost::function<void (const Request*, Response*, bool, int)> callback,
                    int32_t rpc_timeout, int /*retry_times*/) {
    sofa::pbrpc::RpcController* controller = new sofa::pbrpc::RpcController();
    controller->SetTimeout(rpc_timeout * 1000L);
    google::protobuf::Closure* done = 
      sofa::pbrpc::NewClosure(&RpcClient::template RpcCallback<Request, Response, Callback>,
                                          controller, request, response, callback);
    (stub->*func)(controller, request, response, done);
  }
    
  template <class Request, class Response, class Callback>
  static void RpcCallback(sofa::pbrpc::RpcController* rpc_controller,
                            const Request* request,
                            Response* response,
                            boost::function<void (const Request*, Response*, bool, int)> callback) {
    bool failed = rpc_controller->Failed();
    int error = rpc_controller->ErrorCode();
    if (failed || error) {
      assert(failed && error);
      if (error != sofa::pbrpc::RPC_ERROR_SEND_BUFFER_FULL) {
        LOG(WARNING, "RpcCallback: %s\n", rpc_controller->ErrorText().c_str());
      } else {
        ///TODO: Retry
      }
    }
    delete rpc_controller;
    callback(request, response, failed, error);
  }
private:
  sofa::pbrpc::RpcClient* _rpc_client;
  typedef std::map<std::string, sofa::pbrpc::RpcChannel*> HostMap;
  HostMap _host_map;
  ::baidu::common::Mutex _host_map_lock;
};

} // namespace el

#endif /* !RPC_CLIENT_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
