/*
 * segment_indexer.cc
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

#include "segment_indexer.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::DEBUG;
using ::baidu::common::WARNING;

namespace el {


SegmentIndexer::SegmentIndexer(const std::string& filename,
    const std::string& folder,
    bool cache):filename_(filename),folder_(folder), cache_(cache),
  codec_(){}


SegmentIndexer::~SegmentIndexer() {}

bool SegmentIndexer::Init() {
  std::string path = folder_ + "/" + filename_;
  if (cache_) {
    fd_ = fopen(path.c_str(), "rb");
  }else {
    fd_ = fopen(path.c_str(), "ab+");
  }
  if (fd_ == NULL) {
    LOG(WARNING, "fail to create index %s", path.c_str());
    return false;
  }
  /*int file_no = fileno(fd_);
  struct stat sb;
  int ok = fstat(file_no, &sb);
  if (ok != 0) {
    LOG(WARNING, "fail to get index %s stat for %s", path.c_str(), strerror(errno));
    return false;
  }*/
  return true;
}

bool SegmentIndexer::Put(uint64_t offset, uint64_t start,
    uint64_t size) {
  std::string header_buf;
  SegmentHeader header;
  header.offset = start;
  header.data_size = size;
  bool ok = codec_.Encode(header, &header_buf);
  if (!ok) {
    LOG(WARNING, "fail to encode header");
    return false;
  }
  size_t header_size = fwrite(header_buf.data(), sizeof(char), header_buf.size(), fd_);
  if (header_size != header_buf.size()) {
    LOG(WARNING, "fail to write header buf size %ld, real size %ld", header_buf.size(),
            header_size);
    return false;
  }
  int ret = fflush(fd_);
  if (ret != 0) {
    LOG(WARNING, "fail to flush index %s for %s", filename_.c_str(),
        strerror(errno));
    return false;
  }
  return true;
}


}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
