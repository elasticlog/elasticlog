/*
 * file_utils.cc
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#include "file_utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::WARNING;

namespace el {

bool Mkdir(const std::string& path) {
  const int dir_mode = 0777;
  int ret = ::mkdir(path.c_str(), dir_mode); 
  if (ret == 0 || errno == EEXIST) {
    return true; 
  }
  LOG(WARNING, "mkdir %s failed err[%d: %s]", 
        path.c_str(), errno, strerror(errno));
  return false;
}

bool MkdirRecur(const std::string& dir_path) {
  size_t beg = 0;
  size_t seg = dir_path.find('/', beg);
  while (seg != std::string::npos) {
    if (seg + 1 >= dir_path.size()) {
      break; 
    }
    if (!Mkdir(dir_path.substr(0, seg + 1))) {
      return false; 
    }
    beg = seg + 1;
    seg = dir_path.find('/', beg);
  }
  return Mkdir(dir_path);
}

}

