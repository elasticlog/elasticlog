/*
 * segment_indexer.h
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

#ifndef INDEX_APPENDER_H
#define INDEX_APPENDER_H

#include <string>
#include <map>
#include <stdint.h>
#include "segment_codec.h"
#include "base_appender.h"

namespace el {

struct EntryIndex {
  uint64_t start;
  uint64_t size;
};

// thread unsafe
class IndexAppender {

public:
  IndexAppender(const std::string& filename,
      const std::string& folder,
      uint64_t max_size);
  ~IndexAppender();

  // Init segment indexer
  bool Init();

  // index log entry 
  // input
  // offset:log id
  // start :the segment block start offset
  // size  :the entry data size
  bool Put(uint64_t offset, uint64_t start,
           uint64_t size);

  void Close();

private:

  // the cache of index
  std::map<uint64_t, EntryIndex*> idx_;
  SegmentCodec codec_;
  BaseAppender appender_;
};

}

#endif /* !INDEX_APPENDER_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
