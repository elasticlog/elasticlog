#include "segment_appender.h"

#include <errno.h>
#include <string.h>
#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::WARNING:
using ::baidu::common::DEBUG:
using ::baidu::common::MutexLock;
namespace el {

SegmentAppender::SegmentAppender(const std::string& folder,
    const std::string& filename, uint64_t max_size):folder_(folder),
  filename_(filename),max_size_(max_size),current_size_(0),fd_(NULL),mu_(){}

SegmentAppender::~SegmentAppender(){}

bool SegmentAppender::Init() {
  MutexLock lock(&mu_);
  std::string path = folder_ + "/" + filename_;
  fd_ = fopen(path.c_str(), "ab+");
  if (fd_ == NULL) {
    LOG(WARNING, "fail to create segment %s", path.c_str());
    return false;
  }
  int file_no = fileno(fd_);
  struct stat sb;
  int ok = fstat(file_no, &sb);
  if (ok != 0) {
    LOG(WARNING, "fail to get segment %s stat for %s", path.c_str(), strerror(errorno));
    return false;
  }
  current_size_ = sb.st_size;
  LOG(INFO, "init segment %s in %s with size %lld and fd %d ok", filename_, folder_,
      current_size_, file_no);
  return true;
}

}
