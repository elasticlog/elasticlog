/*
 * base_appender.cc
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

#include "base_appender.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::WARNING;
using ::baidu::common::DEBUG;


namespace el {

BaseAppender::BaseAppender(const std::string& filename,
    const std::string& folder):filename_(filename_),
  folder_(folder),
  current_size_(0),
  fd_(NULL) {}

BaseAppender::~BaseAppender() {}


bool BaseAppender::Init() {
  std::string path = folder_ + "/" + filename_;
  fd_ = fopen(path.c_str(), "ab+");
  if (fd_ == NULL) {
    LOG(WARNING, "fail to create file %s", path.c_str());
    return false;
  }
  int file_no = fileno(fd_);
  struct stat sb;
  int ok = fstat(file_no, &sb);
  if (ok != 0) {
    LOG(WARNING, "fail to get file %s stat for %s", path.c_str(), strerror(errno));
    return false;
  }
  current_size_ = sb.st_size;
  LOG(INFO, "init file %s in %s with size %lld and fd %d ok", filename_.c_str(),
          folder_.c_str(),
          current_size_, file_no);
  return true;
}

int64_t BaseAppender::Append(const char* buf) {


}

}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
