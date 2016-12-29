/*
 * segment_reader.cc
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

#include "segment_reader.h"

#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::DEBUG;
using ::baidu::common::WARNING;

namespace el {

const static uint64_t HEADER_SIZE = 2 + 8 * 2;

SegmentReader::SegmentReader(const std::string& folder,
    const std::string& filename):folder_(folder),
  filename_(filename),sid_counter_(0),
  sessions_(),
  idx_cache_(),
  last_entry_id_(0),
  codec_() {}

SegmentReader::~SegmentReader() {}

bool SegmentReader::AddSession(uint32_t* sid) {
  if (sid == NULL) {
    LOG(WARNING, "add session sid is null");
    return false;
  }
  sid_counter_++;
  *sid = sid_counter_;
  std::string path = folder_ + "/" + filename_;
  FILE* fd_ = fopen(path.c_str(), "rb");
  if (fd_ == NULL) {
    LOG(WARNING, "fail to create segment %s", path.c_str());
    return false;
  }
  ReadSession* session = new ReadSession();
  session->sid_ = sid;
  session->last_read_offset_ = 0;
  session->fd_ = fd_;
  sessions_->insert(std::make_pair(sid, session));
  LOG(INFO, "create session %ld of file %s", sid, filename_.c_str());
  return true;
}

bool SegmentReader::Next(LogItem* log_item) {

  std::map<uint64_t, EntryIndex*>::iterator it = idx_cache_.find(last_entry_id_);
  ++it;
  if (it != idx_cache_.end()) {
    uint64_t entry_id = it->first;
    log_item->size = it->second->size;
    log_item->data.resize(log_item->size);
    //check if need seek
    if (current_offset_ == it->second.start) {
      char* dbuf = reinterpret_cast<char*>(& (log_item->data[0]));
      read_size = fread(dbuf, log_item->size, sizeof(char), fd_);
      if (read_size < log_item->size) {
        return false;
      }
      last_entry_id_ = entry_id;
      current_offset_ += read_size;
      return true;
    }else {
      //TODO seek and read
    }
  }
  LOG(WARNING, "fail to find next log to read, last log id %lld", last_entry_id_);
  return false;
}

void SegmentReader::PutIdxCache(uint64_t entry_id, EntryIndex* idx) {
  idx_cache_.insert(std::make_pair(entry_id, idx));
}

bool SegmentReader::Reset(uint64_t offset) {
  return true;
}

void SegmentReader::Close() {

}

}


/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
