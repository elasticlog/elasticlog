#include "segment_appender.h"

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "logging.h"
#include "timer.h"

using ::baidu::common::INFO;
using ::baidu::common::WARNING;
using ::baidu::common::DEBUG;

namespace el {

SegmentAppender::SegmentAppender(const std::string& folder,
    const std::string& segment_name ,
    const std::string& index_name,
    uint64_t segment_max_size,
    uint64_t index_max_size):codec_(),
  appender_(segment_name, folder, segment_max_size),
  idx_appender_(index_name, folder, index_max_size){}

SegmentAppender::~SegmentAppender(){}

bool SegmentAppender::Init() {
  bool ok =  appender_.Init();
  if (!ok) {
    return false;
  }
  ok = idx_appender_.Init();
  return ok;
}

// Append log data to segment
// 1.get current segment offset
bool SegmentAppender::Append(const char* data, uint64_t size, uint64_t offset) {
  if (appender_.IsFull()) {
    return false; 
  }
  uint64_t consumed = ::baidu::common::timer::get_micros();
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
  consumed = ::baidu::common::timer::get_micros() - consumed;
  LOG(DEBUG, "flush data to %s successfully consumed %lld", filename_.c_str(), consumed);
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
