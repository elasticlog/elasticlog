/*
 * ellet_impl.h
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#ifndef ELLET_IMPL_H
#define ELLET_IMPL_H

#include "ellet.pb.h"
#include "mutex.h"

using ::google::protobuf::RpcController;
using ::google::protobuf::Closure;

namespace el {

class ElLetImpl : public ElLet {

public:
  ElLetImpl();
  ~ElLetImpl();
  bool Init();
  void AppendLog(RpcController* controller,
                 const AppendLogRequest* request,
                 AppendLogResponse* response,
                 Closure* done);
private:
  ::baidu::common::Mutex mu_;
};

}

#endif /* !ELLET_IMPL_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
