/*
 * ellet_impl.cc
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#include "ellet_impl.h"

#include <gflags/gflags.h>
#include <sstream>
#include "logging.h"
#include "file_utils.h"

using ::baidu::common::INFO;
using ::baidu::common::DEBUG;
using ::baidu::common::WARNING;

DECLARE_string(ellet_segment_dir);

namespace el {

ElLog::ElLog():log_id(0),
  log_name(),
  partion_id(0),
  primary_endpoint(),
  replica_endpoints(),
  state(kPaused),
  refs_(0),
  segment_ids(),
  appender_(NULL),
  current_segment_id_(0),
  mu_(),
  segment_max_size_(0),
  index_max_size_(0),
  partion_dir_(),
  client_scope_(),
  readers_(),
  segment_range_(),
  start_offset_(){}

ElLog::~ElLog() {
  delete appender_;
}

bool ElLog::Init() {
  MutexLock lock(&mu_);
  //TODO recover segment info from file
  if (segment_ids.size() < 0) {
    LOG(WARNING, "invalid segment ids size for log #id %ld #name %s #partion %ld", log_id, log_name.c_str(),
        partion_id);
    return false;
  }
  std::stringstream ss;
  ss << FLAGS_ellet_segment_dir 
     << "/"
     << log_id
     << "_" << log_name << "_" << partion_id << "/";
  partion_dir_ = ss.str();
  bool ok = MkdirRecur(partion_dir_);
  if (!ok) {
    LOG(WARNING, "fail to create partion dir %s for log #id %ld #name %s", partion_dir_.c_str(),
        log_id, log_name.c_str());
    return false;
  }
  bool rolling_ok = Rolling();
  if (!rolling_ok) {
    return false;
  }
  return true;
}

void ElLog::AddRef() {
  ::baidu::common::atomic_inc(&refs_);
  assert(refs_ > 0);
}

void ElLog::DecRef() {
  if (::baidu::common::atomic_add(&refs_, -1) == 1) {
    assert(refs_ == 0);
    Close();
    delete this;
  }
}


void ElLog::Close() {
  MutexLock lock(&mu_);
  if (appender_ != NULL) {
    appender_->Close();
    delete appender_;
    appender_ = NULL;
  }
}

Status ElLog::Append(const char* data, uint64_t size, uint64_t offset) {
  MutexLock lock(&mu_);
  bool ok = false;
  if (appender_ != NULL) {
    ok = appender_->Append(data, size, offset);
    if (ok) {
      return kOk;
    }
    return kAppendError;
  }
  ok = Rolling();
  if (!ok) {
    return kNoSegmentAvailable;
  }
  ok = appender_->Append(data, size, offset);
  if (ok) {
    return kOk;
  }
  return kAppendError;
}

Status ElLog::ReadLog(uint64_t client_id, uint64_t log_id, uint32_t partion_id,
    uint64_t offset,
    ReadLogResponse* response) {
  MutexLock lock(&mu_);
  std::map<uint64_t, uint64_t>::iterator rit = segment_range_.begin();
  uint64_t last_segment_id = 0;
  for (; rit != segment_range_.end(); ++rit) {
    if (offset >= rit->first) {
      last_segment_id = rit->second;
    }else {
      break;
    }
  }
  if (last_segment_id <= 0) {
    LOG(WARNING, "Can not find segment id with #offset %lld", offset);
    response->set_status(kSegmentNotFound);
    return;
  }
  std::map<uint64_t, SegmentReader*>::iterator sit = readers_.find(last_segment_id);
  if (sit == readers_.end()) {
    LOG(WARNING, "Can not find segment reader with #id %lld", last_segment_id);
    response->set_status(kSegmentNotFound);
    return;
  }
  SegmentReader* reaader = sit->second;
  std::map<uint64_t, uint32_t>::iterator it = client_scope_.find(client_id);
  uint32_t sid = 0;
  if (it == client_scope_.end()) {
    bool ok = reader->NewScope(&sid);
    if (!ok) {
      LOG(WARNING, "fail to create a read scope for client #id %lld", client_id);
      response->set_status(kRpcErr);
      return;
    }
  }
  sid = it->second;

}

bool ElLog::Rolling() {
  mu_.AssertHeld();
  if (appender_ != NULL) {
    appender_->Close();
    delete appender_;
    appender_ = NULL;
  }
  std::vector<uint64_t>::iterator it = segment_ids.begin();
  bool ret = false;
  for (; it != segment_ids.end(); ++it) {
    uint64_t it_val = *it;
    if (current_segment_id_ <= 0) {
      current_segment_id_ = it_val;
      ret = true;
      break;
    }
    if (current_segment_id_ == it_val) {
      ++it;
      if (it != segment_ids.end()) {
        current_segment_id_ = *it;
        ret = true;
      }
      break;
    }
  }
  if (ret) {
    std::stringstream segment_ss;
    segment_ss << current_segment_id_ << ".segm";
    std::stringstream idx_ss;
    idx_ss << current_segment_id_ << ".idx";
    std::string segment_name = segment_ss.str();
    std::string index_name = idx_ss.str();
    appender_ = new SegmentAppender(partion_dir_, segment_name,index_name,
        segment_max_size_,
        index_max_size_);
    bool ok = appender_->Init();
    if (!ok) {
      delete appender_;
      appender_ = NULL;
      return false;
    }
    SegmentReader* reader = new SegmentReader(partion_dir_, index_name);
    readers_.insert(std::make_pair(current_segment_id_, reader));
    LOG(INFO, "rolling next segment #id %ld #succ %d for log #id %ld #name %s", 
      current_segment_id_, ret,
      log_id, log_name.c_str());
  }
  return ret;
}

ElLetImpl::ElLetImpl():mu_(),el_logs_(){}

ElLetImpl::~ElLetImpl() {}

bool ElLetImpl::Init() {
  return true;
}

void ElLetImpl::AppendEntry(RpcController* controller,
                 const AppendEntryRequest* request,
                 AppendEntryResponse* response,
                 Closure* done) {
  ElLog* el_log = NULL;
  {
    MutexLock lock(&mu_);
    ElLogs::iterator it = el_logs_.find(request->entry().log_id());
    if (it == el_logs_.end()) {
      LOG(WARNING, "fail to find log with #id %ld", request->entry().log_id());
      response->set_status(kLogNotFound);
      done->Run();
      return;
    }
    el_log = it->second;
    el_log->AddRef();
  }
  Status ok = el_log->Append(request->entry().content().c_str(),
      request->entry().content().size(),1);
  response->set_status(ok);
  done->Run();
  el_log->DecRef();
}

void ElLetImpl::ReadLog(RpcController* controller,
                const ReadLogRequest* request,
                ReadLogResponse* response,
                Closure* done) {
  ElLog* el_log = NULL;
  {
    MutexLock lock(&mu_);
    ElLogs::iterator it = el_logs_.find(request->log_id());
    if (it == el_logs_.end()) {
      LOG(WARNING, "fail to find log with #id %ld", request->log_id());
      response->set_status(kLogNotFound);
      done->Run();
      return;
    }
    el_log = it->second;
    el_log->AddRef();
  }
  el_log->ReadLog()

}

void ElLetImpl::DeploySegment(RpcController* controller,
                 const DeploySegmentRequest* request,
                 DeploySegmentResponse* response,
                 Closure* done) {
  ElLog* el_log = new ElLog();
  el_log->log_id = request->log_id();
  el_log->log_name = request->log_name();
  el_log->partion_id = request->partion_id();
  el_log->primary_endpoint = request->primary_endpoint();
  el_log->segment_max_size_ = request->segment_max_size();
  el_log->index_max_size_ = request->idx_max_size();
  el_log->start_offset_ = request->start_offset();
  std::stringstream ids;
  for (int i = 0; i < request->replica_endpoints_size(); ++i) {
    el_log->replica_endpoints.insert(request->replica_endpoints(i));
  }
  for (int i = 0; i < request->segment_ids_size(); i++) {
    if (i > 0) {
      ids << ",";
    }
    el_log->segment_ids.push_back(request->segment_ids(i));
    ids << request->segment_ids(i);
  }
  el_log->state = request->state();
  el_log->AddRef();
  MutexLock lock(&mu_);
  ElLogs::iterator it = el_logs_.find(request->log_id());
  if (it != el_logs_.end()) {
    LOG(WARNING, "el log with id %ld and name %s does exists", request->log_id(),
        request->log_name().c_str());
    el_log->DecRef();
    response->set_status(kLogExists);
    done->Run();
    return;
  }
  bool ok = el_log->Init();
  if (!ok) {
    el_log->DecRef();
    response->set_status(kInitElLogError);
    done->Run();
    return;
  }
  el_logs_.insert(std::make_pair(request->log_id(), el_log));
  LOG(INFO, "add log with id %lld, name %s and state %s #segments %s successfully", request->log_id(),
      request->log_name().c_str(),
      ElLogState_Name(el_log->state).c_str(),
      ids.str().c_str());
  response->set_status(kOk);
  done->Run();
}

}


/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
