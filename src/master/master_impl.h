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
#include "mutex.h"
#include "counter.h"

using ::google::protobuf::RpcController;
using ::google::protobuf::Closure;
using ::baidu::common::Mutex;
using ::baidu::common::MutexLock;


namespace el {

class MasterImpl : public ElMaster {

public:
  MasterImpl();
  ~MasterImpl();

  void CreateLog(RpcController* controller,
                const CreateLogRequest* request,
                CreateLogResponse* response,
                Closure* done);
};

}

#endif /* !MASTER_IMPL_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
