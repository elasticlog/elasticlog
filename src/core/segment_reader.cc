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

namespace el {

const static uint64_t HEADER_SIZE = 10 + 8 * 2;

SegmentReader::SegmentReader(const std::string& folder,
    const std::string& filename):folder_(folder),
  filename_(filename), fd_(NULL),codec_() {}

SegmentReader::~SegmentReader() {}

bool SegmentReader::Init() {

  return false;
}

bool SegmentReader::Next(LogItem* log_item) {
  std::string header;
  header.resize(HEADER_SIZE);
  //TODO guarantee read next log item
  char* hbuf = reinterpret_cast<char*>(& (header[0]));
  size_t read_size = fread(hbuf, HEADER_SIZE, sizeof(char), fd_);
  if (read_size < HEADER_SIZE) {
    // reset
    return false;
  }
  SegmentHeader sheader;
  bool ok = codec_.Decode(&header, &sheader);
  log_item->size = sheader.data_size;
  log_item->offset = sheader.offset;
  log_item->data.resize(sheader.data_size);
  char* dbuf = reinterpret_cast<char*>(& (log_item->data[0]));
  read_size = fread(dbuf, log_item->size, sizeof(char), fd_);
  if (read_size < log_item->size) {
    return false;
  }
  return true;
}

bool SegmentReader::Reset(uint64_t offset) {
  return true;
}

void SegmentReader::Close() {

}

}


/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
