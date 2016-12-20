/*
 * master_impl.cc
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

#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::WARNING;
using ::baidu::common::DEBUG;

namespace el {

LogInfo::LogInfo():log_name_(),
  log_id_(0),
  partion_count_(0),
  partion_replica_(0),
  state_(kPaused),
  refs_(0),
  segments_(),
  mu_(){}

LogInfo::~LogInfo() {}

void LogInfo::AddRef() {
  ::baidu::common::atomic_inc(&refs_);
  assert(refs_ > 0);
}

void LogInfo::DecRef() {
  if (::baidu::common::atomic_add(&refs_, -1) == 1) {
    assert(refs_ == 0);
    delete this;
  }
}

MasterImpl::MasterImpl() {}

MasterImpl::~MasterImpl() {}

void MasterImpl::CreateLog(RpcController* controller,
                           const CreateLogRequest* request,
                           CreateLogResponse* response,
                           Closure* done){
  LogInfo* log = new LogInfo();
  log->log_id_ = ++log_id_counter_;
  log->log_name_ = request->log_name();
  log->partion_count_ = request->partion_count();
  log->partion_replica_ = request->partion_replica();
  log->AddRef();
  MutexLock lock(&mu_);
  std::map<uint64_t, LogInfo*>::iterator it = logs_.find(log->log_id_);
  if (it != logs_.end()) {
    LOG(WARNING, "fail to create log with #id %lld #name %s #partion_count %d #partion_replica %d",
        log->log_id_, log->log_name_.c_str(), log->partion_count_, log->partion_replica_);
    log->DecRef();
    response->set_status(kLogExists);
    done->Run();
    return;
  }
  logs_.insert(std::make_pair(log->log_id_, log));
  response->set_status(kOk);
  done->Run();
  LOG(WARNING, "create log with #id %lld #name %s #partion_count %d #partion_replica %d successfully",
        log->log_id_, log->log_name_.c_str(), log->partion_count_, log->partion_replica_);
}

}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
