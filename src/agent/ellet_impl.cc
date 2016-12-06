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
  appenders_(),
  current_segment_id_(0),
  mu_(),
  segment_ids_size_(0){}
ElLog::~ElLog() {}

bool ElLog::Init() {
  MutexLock lock(&mu_);
  //TODO recover segment info from file
  if (segment_ids.size() < 0) {
    LOG(WARNING, "invalid segment ids size for log #id %lld #name %s #partion %lld", log_id, log_name.c_str(),
        partion_id);
    return false;
  }
  current_segment_id_ = segment_ids.begin();
  std::string partion_dir = FLAGS_ellet_segment_dir + "/" + log_id + "/" + partion_id;
  bool ok = MkdirRecur(partion_dir);
  if (!ok) {
    LOG(WARNING, "fail to create partion dir %s for log #id %lld #name %s", partion_dir.c_str(),
        log_id, log_name.c_str());
    return false;
  }
  std::string segment_name = current_segment_id_ + ".segm";
  //TODO check which segment should be init
  SegmentAppender* appender = new SegmentAppender(partion_dir, segment_name,
      segment_max_size_);
  ok = appender->Init();
  if (!ok) {
    LOG(WARNING, "fail to init segment #id %lld of log #id %lld #name %s",
        current_segment_id_, log_id, log_name.c_str());
    return false;
  }
  appenders_.insert(std::make_pair(current_segment_id_, appender));
  return true;
}

void ElLog::AddRef() {
  ::baidu::common::atomic_inc(&refs_);
  assert(refs_ > 0);
}

void ElLog::DecRef() {
  if (::baidu::common::atomic_add(&refs_, -1) == 1) {
    assert(refs_ == 0);
    delete this;
  }
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
      LOG(WARNING, "fail to find log with id %lld", request->entry().log_id());
      response->set_status(kLogNotFound);
      done->Run();
      return;
    }
    el_log = it->second;
    el_log->AddRef();
  }
  
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
  for (int i = 0; i < request->replica_endpoints_size(); ++i) {
    el_log->replica_endpoints.insert(request->replica_endpoints(i));
  }
  for (int i = 0; i < request->segment_ids_size(); i++) {
    el_log->segment_ids.insert(request->segment_ids(i));
  }
  el_log->state = request->state();
  el_log->AddRef();
  MutexLock lock(&mu_);
  ElLogs::iterator it = el_logs_.find(request->log_id());
  if (it != el_logs_.end()) {
    LOG(WARNING, "el log with id %lld and name %s does exists", request->log_id(),
        request->log_name().c_str());
    el_log->DecRef();
    response->set_status(kLogExists);
    done->Run();
    return;
  }
  el_logs_.insert(std::make_pair(request->log_id(), el_log));
  LOG(INFO, "add log with id %lld, name %s and state %s successfully", request->log_id(),
      request->log_name().c_str(),
      ElLogState_Name(el_log->state).c_str());
  response->set_status(kOk);
  done->Run();
}


}


/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
