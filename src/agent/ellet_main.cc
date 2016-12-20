/*
 * ellet_main.cc
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#include "ellet_impl.h"

#include <gflags/gflags.h>
#include <sofa/pbrpc/pbrpc.h>
#include "logging.h"
#include <signal.h>
#include <unistd.h>
#include "version.h"

using ::baidu::common::INFO;
using ::baidu::common::WARNING;
using ::baidu::common::DEBUG;

DECLARE_string(ellet_endpoint);
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
      printf("ellet version %d.%d \n", EL_VERSION_MAJOR, EL_VERSION_MINOR);
      return 0;
    }
  }
  ::baidu::common::SetLogFile("./ellet.log", true);
  ::baidu::common::SetWarningFile("./ellet.wlog", true);
  ::baidu::common::SetLogSize(1024);//1024M
  ::google::ParseCommandLineFlags(&argc, &args, true);
  sofa::pbrpc::RpcServerOptions options;
  sofa::pbrpc::RpcServer rpc_server(options);  
  ::el::ElLetImpl* ellet = new ::el::ElLetImpl();
  ellet->Init();
  ::google::ShutDownCommandLineFlags();
  if (!rpc_server.RegisterService(ellet)) {
    LOG(WARNING, "fail to start ellet service");
    exit(1);
  }
  if (!rpc_server.Start(FLAGS_ellet_endpoint)) {
    LOG(WARNING, "fail to listen endpoint %s", FLAGS_ellet_endpoint.c_str());
    exit(1);
  }
  LOG(INFO, "start ellet with endpoint %s successfully", FLAGS_ellet_endpoint.c_str());
  signal(SIGINT, SignalIntHandler);
  signal(SIGTERM, SignalIntHandler);
  while (!s_quit) {
    sleep(1);
  }
  return 0;
}



/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
