/*
 * ellet_impl.cc
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#include "ellet_impl.h"

namespace el {

ElLetImpl::ElLetImpl():mu_(){}

ElLetImpl::~ElLetImpl() {}

bool ElLetImpl::Init() {
  return true;
}

void ElLetImpl::AppendLog(RpcController* controller,
                 const AppendLogRequest* request,
                 AppendLogResponse* response,
                 Closure* done) {


}

}


/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
