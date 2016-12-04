/*
 * ellet_impl.h
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#ifndef ELLET_IMPL_H
#define ELLET_IMPL_H

#include <set>
#include <map>
#include "ellet.pb.h"
#include "mutex.h"
#include "counter.h"

using ::google::protobuf::RpcController;
using ::google::protobuf::Closure;
using ::baidu::common::Mutex;
using ::baidu::common::MutexLock;

namespace el {

struct ElLog {
  uint64_t log_id;
  std::string log_name;
  uint32_t partion_id;
  std::string primary_endpoint;
  std::set<std::string> replica_endpoints;
  ElLogState state;
  volatile int refs_;
  std::set<uint64_t> segment_ids;

  void AddRef() {
    ::baidu::common::atomic_inc(&refs_);
    assert(refs_ > 0);
  }

  void DecRef() {
    if (::baidu::common::atomic_add(&refs_, -1) == 1) {
      assert(refs_ == 0);
      delete this;
    }
  }
};

typedef std::map<uint64_t, ElLog*> ElLogs;

class ElLetImpl : public ElLet {

public:
  ElLetImpl();
  ~ElLetImpl();
  bool Init();
  void DeploySegment(RpcController* controller,
                 const DeploySegmentRequest* request,
                 DeploySegmentResponse* response,
                 Closure* done);
  void AppendEntry(RpcController* controller,
                 const AppendEntryRequest* request,
                 AppendEntryResponse* response,
                 Closure* done);
private:
  Mutex mu_;
  ElLogs el_logs_;
};

}

#endif /* !ELLET_IMPL_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
