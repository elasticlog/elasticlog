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

#include <errno.h>
#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::DEBUG;
using ::baidu::common::WARNING;

namespace el {

const static uint64_t HEADER_SIZE = 2 + 8 * 2;

SegmentReader::SegmentReader(const std::string& folder,
    const std::string& filename):folder_(folder),
  filename_(filename),sid_counter_(0),
  scopes_(),
  idx_cache_(),
  codec_() {}

SegmentReader::~SegmentReader() {}

bool SegmentReader::NewScope(uint32_t* sid) {
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
  ReadScope* scope = new ReadScope();
  scope->sid_ = *sid;
  scope->last_read_offset_ = 0;
  scope->fd_ = fd_;
  scope->last_entry_id_ = 0;
  scopes_.insert(std::make_pair(scope->sid_, scope));
  LOG(INFO, "create scope %ld of file %s", scope->sid_, filename_.c_str());
  return true;
}

bool SegmentReader::Next(uint32_t sid, LogItem* log_item) {
  if (log_item == NULL) {
    return false;
  }
  std::map<uint32_t, ReadScope*>::iterator sit = scopes_.find(sid);
  if (sit == scopes_.end()) {
    LOG(WARNING, "fail find read scope with #id $ld", sid);
    return false;
  }
  ReadScope* scope = sit->second;
  std::map<uint64_t, EntryIndex*>::iterator it = idx_cache_.begin(); 
  if (scope->last_entry_id_ > 0) {
    it = idx_cache_.find(scope->last_entry_id_);
    ++it;
  }
  if (it != idx_cache_.end()) {
    uint64_t entry_id = it->first;
    log_item->size = it->second->size;
    log_item->data.resize(log_item->size);
    if (scope->last_read_offset_ != it->second->start) {
      //TODO seek and read
      int ok = fseek(scope->fd_, it->second->start, SEEK_SET);
      if (ok != 0) {
        LOG(WARNING, "fail to seek to %lld with file %s for %s",
            it->second->start,
            filename_.c_str(),
            strerror(errno));
        return false;
      }
      LOG(DEBUG, "miss file iterator and seek to %lld with file %s",
        it->second->start,
        filename_.c_str());
      scope->last_read_offset_ = it->second->start;
    }
    char* dbuf = reinterpret_cast<char*>(& (log_item->data[0]));
    size_t read_size = fread(dbuf, sizeof(char), log_item->size, scope->fd_);
    if (read_size < log_item->size) {
      LOG(WARNING, "fail to read expect size %ld from %s, actual %ld",
          log_item->size, filename_.c_str(), read_size);
      return false;
    }
    scope->last_entry_id_ = entry_id;
    scope->last_read_offset_ += read_size;
    LOG(DEBUG, "use scan to read next log for #entry %lld #read offset %lld",
        scope->last_entry_id_,
        scope->last_read_offset_);
    return true;
  }
  LOG(WARNING, "fail to find next log to read, last log id %lld", scope->last_entry_id_);
  return false;
}

void SegmentReader::PutIdxCache(uint64_t entry_id, EntryIndex* idx) {
  idx_cache_.insert(std::make_pair(entry_id, idx));
}


void SegmentReader::Close() {

}

}


/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
