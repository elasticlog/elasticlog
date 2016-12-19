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


}


}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
