/*
 * ellet_impl.h
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#ifndef ELLET_IMPL_H
#define ELLET_IMPL_H

#include <vector>
#include <map>
#include "ellet.pb.h"
#include "mutex.h"
#include "counter.h"
#include "segment_appender.h"
#include "segment_reader.h"

using ::google::protobuf::RpcController;
using ::google::protobuf::Closure;
using ::baidu::common::Mutex;
using ::baidu::common::MutexLock;

namespace el {

class ElLetImpl;

class ElLog {

public:
  ElLog();
  ~ElLog();

  bool Init();

  void AddRef();
  void DecRef();

  Status Append(const char* data, 
                uint64_t size,
                uint64_t offset);

  Status ReadLog(uint64_t client_id,
      uint64_t log_id,
      uint32_t partion_id,
      uint64_t offset,
      ReadLogResponse* response);

private:
  // create a new segment for next segment ids;
  bool Rolling();
  void Close();

private:

  uint64_t log_id;
  std::string log_name;
  uint32_t partion_id;
  std::string primary_endpoint;
  std::set<std::string> replica_endpoints;
  ElLogState state;
  volatile int refs_;
  std::vector<uint64_t> segment_ids;
  SegmentAppender* appender_;
  uint64_t current_segment_id_;
  Mutex mu_;
  uint64_t segment_max_size_;
  uint64_t index_max_size_;
  std::string partion_dir_;
  std::map<uint64_t, uint32_t> client_scope_;
  // segment id 
  std::map<uint64_t, SegmentReader*> readers_;
  //  
  std::map<uint64_t, uint64_t> segment_range_;
  uint64_t start_offset_;
  friend class ElLetImpl;
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

  void ReadLog(RpcController* controller,
                const ReadLogRequest* request,
                ReadLogResponse* response,
                Closure* done);

private:
  Mutex mu_;
  ElLogs el_logs_;
};

}

#endif /* !ELLET_IMPL_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
