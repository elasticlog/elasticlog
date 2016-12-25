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

#include "index_appender.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::DEBUG;
using ::baidu::common::WARNING;

namespace el {


IndexAppender::IndexAppender(const std::string& filename,
    const std::string& folder,
    uint64_t max_size):codec_(),appender_(filename, folder, max_size){}

IndexAppender::~IndexAppender() {}

bool IndexAppender::Init() { 
  return appender_.Init();
}

bool IndexAppender::Put(uint64_t offset, uint64_t start,
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
  
  //TODO slidewindow
  int64_t write_size = appender_.Append(header_buf.data(), 
      header_buf.size());
  if (write_size != header_buf.size()) {
    LOG(WARNING, "fail to append index");
    return false;
  }
  appender_.Flush();
  return true;
}

bool IndexAppender::Sync() {
  return appender_.Sync();
}

void IndexAppender::Close() {
  appender_.Sync();
  appender_.Close();
}

}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
