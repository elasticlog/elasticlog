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

#ifndef SEGMENT_INDEXER_H
#define SEGMENT_INDEXER_H

#include <string>
#include <map>
#include <stdint.h>

namespace el {

struct EntryIndex {
  uint64_t start;
  uint64_t size;
};

class SegmentIndexer {

public:
  SegmentIndexer();
  ~SegmentIndexer();

  // Init segment indexer
  bool Init();

  // index log entry 
  // input
  // offset:log id
  // start :the segment block start offset
  // size  :the entry data size
  bool Put(uint64_t offset, uint64_t start,
           uint64_t size);

  // Get index log meta
  // input
  // offset:log id
  // output
  // start :the segment block start offset
  // size  :the entry data size
  bool Get(uint64_t offset, uint64_t* start,
      uint64_t* size);

private:
  std::string filename_;
  std::string folder_;

  FILE* fd_;

  // the cache of index
  std::map<uint64_t, EntryIndex*> idx_;
};

}

#endif /* !SEGMENT_INDEXER_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
