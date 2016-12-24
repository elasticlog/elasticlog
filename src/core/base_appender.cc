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
    const std::string& folder, uint64_t max_size):filename_(filename),
  folder_(folder),
  current_size_(0),
  max_size_(max_size),
  fd_(NULL),
  fd_no_(0){}

BaseAppender::~BaseAppender() {}

bool BaseAppender::Init() {
  std::string path = folder_ + "/" + filename_;
  fd_ = fopen(path.c_str(), "ab+");
  if (fd_ == NULL) {
    LOG(WARNING, "fail to create file %s", path.c_str());
    return false;
  }
  fd_no_ = fileno(fd_);
  struct stat sb;
  int ok = fstat(fd_no_, &sb);
  if (ok != 0) {
    LOG(WARNING, "fail to get file %s stat for %s", path.c_str(), strerror(errno));
    return false;
  }
  current_size_ = sb.st_size;
  LOG(INFO, "init file %s in %s with size %lld and fd %d ok", filename_.c_str(),
          folder_.c_str(),
          current_size_, fd_no_);
  return true;
}

int64_t BaseAppender::Append(const char* buf, uint64_t size) {
  if (current_size_ >= max_size_) {
    LOG(WARNING,  "file %s reaches the max size %lld", filename_.c_str(),
        max_size_);
    return -1;
  }
  size_t data_size = fwrite(buf, sizeof(char), size, fd_);
  if (data_size > 0) {
    current_size_ += data_size;
  }
  if (data_size <= 0) {
    LOG(WARNING, "fail to write buf to file %s for %s", filename_.c_str(),
        strerror(errno));
  }
  return data_size;
}

bool BaseAppender::Flush() {
  int ok  = fflush(fd_);
  if (ok != 0) {
    LOG(WARNING, "fail to flush buf to file %s for %s",
        filename_.c_str(), strerror(errno));
    return false;
  }
  LOG(DEBUG, "flush write buf to file %s successfully", filename_.c_str());
}

bool BaseAppender::Sync() {

  if (fd_no_ <= 0) {
    return false;
  }

  int ok = fsync(fd_no_);
  if (ok != 0) {
    LOG(WARNING, "fail to fsync fd %ld with filename %s for %s",
        fd_no_, filename_.c_str(), strerror(errno));
    return false;
  }
  LOG(DEBUG, "sync file %s successfully", filename_.c_str());
  return true;

}

void BaseAppender::Close() {
  if (fd_ == NULL) {
    return; 
  }
  int ret = fclose(fd_);
  if (ret != 0) {
    LOG(WARNING, "fail to close filename %s for %s", filename_.c_str(),
        strerror(errno));
    return;
  }
  LOG(DEBUG, "close file %s successfully", filename_.c_str());
  fd_ = NULL;
}

bool BaseAppender::IsFull() {
  return current_size_ >= max_size_;
}

//TODO find a better way to get position
uint64_t BaseAppender::Position() {
  return current_size_;
}

}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
