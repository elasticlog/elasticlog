/*
 * base_appender.h
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

#ifndef BASE_APPENDER_H
#define BASE_APPENDER_H

#include <stdio.h>
#include <string>

namespace el {

//Note: not thread safe
class BaseAppender {

public:
  BaseAppender(const std::string& filename,
      const std::string& folder);
  ~BaseAppender();
  bool Init();
  int64_t Append(const char* buf);
  bool Flush();
  bool Sync();
  void Close();
private:
  std::string filename_;
  std::string folder_;
  uint64_t current_size_;
  FILE* fd_;
};

}

#endif /* !BASE_APPENDER_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
