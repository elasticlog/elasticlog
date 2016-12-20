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

SegmentReader::SegmentReader(const std::string& folder,
    const std::string& filename):folder_(folder),
  filename_(filename) {}

SegmentReader::~SegmentReader() {}

bool SegmentReader::Init() {

  return false;
}

bool SegmentReader::Next(LogItem* log_item) {
  return true;
}

bool SegmentReader::Reset(uint64_t offset) {
  return true;
}

void SegmentReader::Close() {

}

}


/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
