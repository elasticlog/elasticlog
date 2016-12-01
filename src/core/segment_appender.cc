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
  filename_(filename),max_size_(max_size),current_size_(0),fd_(NULL),mu_(),
    codec_(){}

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

bool SegmentAppender::Append(const char* data, uint64_t size, uint64_t offset) {
  std::string header_buf;
  SegmentHeader header;
  header.offset = offset;
  header.size = size;
  bool ok = codec.Encode(header, &header_buf);
  MutexLock lock(&mu_);
  int header_size = fwrite(header_buf.data(), sizeof(char), header_buf.size(), fd_);
  if (header_size != header_buf.size()) {
    LOG(WARNING, "fail to write header buf size %ld, real size %ld", header_size.size(),
            header_size);
    return false;
  }
  int data_size = fwrite(data, sizeof(char), size, fd_);
  if (data_size != size) {
    LOG(WARNING, "fail to write data buf size %ld, real size %ld", size, data_size);
    return false;
  }
  fflush(fd_);
  return true;
}

}
