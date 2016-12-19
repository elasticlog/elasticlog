/*
 * master_main.cc
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

#include "master_impl.h"

#include <gflags/gflags.h>
#include <sofa/pbrpc/pbrpc.h>
#include "logging.h"
#include <signal.h>
#include <unistd.h>
#include "version.h"

using ::baidu::common::INFO;
using ::baidu::common::WARNING;
using ::baidu::common::DEBUG;

DECLARE_string(elmaster_endpoint);
DECLARE_bool(version);
static volatile bool s_quit = false;
static void SignalIntHandler(int /*sig*/){
  s_quit = true;
}

int main(int argc, char* args[]) {
  std::string version = EL_VERSION_MAJOR + "." + EL_VERSION_MINOR;
  if (argc > 1) {
    std::string cmd = args[1];
    if (cmd == "version"
        || cmd == "--version"
        || cmd == "-v") {
      printf("elmaster version %d.%d \n", EL_VERSION_MAJOR, EL_VERSION_MINOR);
      return 0;
    }
  }
  ::baidu::common::SetLogFile("./elmaster.log", true);
  ::baidu::common::SetWarningFile("./elmaster.wlog", true);
  ::baidu::common::SetLogSize(1024);//1024M
  ::google::ParseCommandLineFlags(&argc, &args, true);
  sofa::pbrpc::RpcServerOptions options;
  sofa::pbrpc::RpcServer rpc_server(options);  
  ::google::ShutDownCommandLineFlags();
  el::MasterImpl* master = new el::MasterImpl();
  if (!rpc_server.RegisterService(master)) {
    LOG(WARNING, "fail to start ellet service");
    exit(1);
  }
  if (!rpc_server.Start(FLAGS_elmaster_endpoint)) {
    LOG(WARNING, "fail to listen endpoint %s", FLAGS_elmaster_endpoint.c_str());
    exit(1);
  }
  LOG(INFO, "start elmaster with endpoint %s successfully", FLAGS_elmaster_endpoint.c_str());
  signal(SIGINT, SignalIntHandler);
  signal(SIGTERM, SignalIntHandler);
  while (!s_quit) {
    sleep(1);
  }
  return 0;
}


/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
