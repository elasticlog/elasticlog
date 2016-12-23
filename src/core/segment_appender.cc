#include "segment_appender.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "logging.h"

using ::baidu::common::INFO;
using ::baidu::common::WARNING;
using ::baidu::common::DEBUG;

namespace el {

SegmentAppender::SegmentAppender(const std::string& folder,
    const std::string& filename, uint64_t max_size):codec_(),
  appender_(filename, folder, max_size){}

SegmentAppender::~SegmentAppender(){}

bool SegmentAppender::Init() {
  return appender_.Init();
}

bool SegmentAppender::Appendable() {
  return max_size_ > current_size_;
}

bool SegmentAppender::Append(const char* data, uint64_t size, uint64_t offset) {
  if (appender_.IsFull()) {
    return false; 
  }
  std::string header_buf;
  SegmentHeader header;
  header.offset = offset;
  header.data_size = size;
  bool ok = codec_.Encode(header, &header_buf);
  int64_t header_size = appender_.Append(header_buf.data(), header_buf.size());
  if (header_size != header_buf.size()) {
    LOG(WARNING, "fail to write header buf size %ld, real size %ld", header_buf.size(),
            header_size);
    return false;
  }
  int data_size = fwrite(data, sizeof(char), size, fd_);
  if (data_size != size) {
    LOG(WARNING, "fail to write data buf size %ld, real size %ld", size, data_size);
    return false;
  }
  current_size_ += header_size + data_size;
  int ret = fflush(fd_);
  if (ret != 0) {
    LOG(WARNING, "fail to flush file %s for %s ", filename_.c_str(), strerror(errno));
    return false;
  }
  LOG(DEBUG, "flush data to %s successfully", filename_.c_str());
  return true;
}

bool SegmentAppender::Sync() {
  if (fd_ == NULL) {
    return false; 
  }
  int file_no = fileno(fd_);
  int ret = fsync(file_no);
  if (ret != 0) {
    LOG(WARNING, "fail to fsync fd %ld with filename %s for %s",
        file_no, filename_.c_str(), strerror(errno));
    return false;
  }
  LOG(DEBUG, "sync segment file %s successfully", filename_.c_str());
  return true;
}

void SegmentAppender::Close() {
  if (fd_ == NULL) {
    return; 
  }
  int ret = fclose(fd_);
  if (ret != 0) {
    LOG(WARNING, "fail to close filename %s for %s", filename_.c_str(),
        strerror(errno));
    return;
  }
  LOG(DEBUG, "close segment file %s successfully", filename_.c_str());
  fd_ = NULL;
}

}
