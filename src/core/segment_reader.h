/*
 * segment_reader.h
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

#ifndef SEGMENT_READER_H
#define SEGMENT_READER_H

#include <stdio.h>
#include <stdint.h>
#include <map>
#include "segment_codec.h"
#include "index_appender.h"
#include "ellet.pb.h"

namespace el {

class SegmentReader {

public:

  SegmentReader(const std::string& folder,
                const std::string& filename);

  ~SegmentReader();

  //scan logs from start entry id 
  bool Scan(uint64_t start_offset, std::vector<LogEntry*> logs);

  //put entry id index
  void PutIdx(uint64_t entry_id, EntryIndex* idx);

  //load all idx to memory
  void LoadIdx();

  //close all fds
  void Close();

private:

  bool LocateEntry(uint64_t entry_id);
private:
  std::string folder_;
  std::string filename_;
  //the file offset
  uint64_t last_read_offset_;
  //the last log entry id
  uint64_t last_entry_id_;
  //segment file descriptor
  FILE* fd_;
  std::map<uint64_t, EntryIndex*> idx_cache_;
  SegmentCodec codec_;
};

}

#endif /* !SEGMENT_READER_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
