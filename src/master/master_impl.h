/*
 * master_impl.h
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

#ifndef MASTER_IMPL_H
#define MASTER_IMPL_H

#include "elmaster.pb.h"

#include <vector>
#include <map>
#include <set>
#include "mutex.h"
#include "counter.h"

using ::google::protobuf::RpcController;
using ::google::protobuf::Closure;
using ::baidu::common::Mutex;
using ::baidu::common::MutexLock;


namespace el {

class MasterImpl;

struct SegmentInfo {
  uint64_t segment_id_;
  uint32_t partion_id_;
  uint64_t log_id_;
  std::string primary_endpoint_;
  std::set<std::string> replica_endpoints_;
};

typedef std::map<uint32_t, std::map<uint64_t, SegmentInfo* > > Segments;

class LogInfo {

public:
  LogInfo();
  ~LogInfo();

  void AddRef();
  void DecRef();

private:
  std::string log_name_;
  uint64_t log_id_;
  uint32_t partion_count_;
  uint32_t partion_replica_;
  ElLogState state_;
  volatile int refs_;
  Segments segments_;
  Mutex mu_;
  friend class MasterImpl;
};


class MasterImpl : public ElMaster {

public:
  MasterImpl();
  ~MasterImpl();

  void CreateLog(RpcController* controller,
                const CreateLogRequest* request,
                CreateLogResponse* response,
                Closure* done);

private:
  Mutex mu_;
  std::map<uint64_t, LogInfo*> logs_;
};

}

#endif /* !MASTER_IMPL_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
