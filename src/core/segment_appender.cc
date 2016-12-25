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
    uint64_t index_max_size):segment_name_(segment_name),codec_(),
  data_appender_(segment_name, folder, segment_max_size),
  idx_appender_(index_name, folder, index_max_size){}

SegmentAppender::~SegmentAppender(){}

bool SegmentAppender::Init() {
  bool ok =  data_appender_.Init();
  if (!ok) {
    return false;
  }
  ok = idx_appender_.Init();
  return ok;
}

// Append log data to segment
// 1.get current segment offset
// 2.write data to segment file
// 3.write index file
bool SegmentAppender::Append(const char* data, uint64_t size, uint64_t entry_id) {
  return Append(data, size, entry_id, NULL); 
}

bool SegmentAppender::Append(const char* data, uint64_t size, uint64_t entry_id,
    EntryIndex* idx) {
  if (data_appender_.IsFull()) {
    LOG(WARNING, "segment file %s is full", segment_name_.c_str());
    return false; 
  }
  uint64_t consumed = ::baidu::common::timer::get_micros();
  uint64_t start = data_appender_.Position();
  int64_t write_size = data_appender_.Append(data, size);
  if (write_size != size) {
    LOG(WARNING, "fail to write data buf size %ld, real size %ld", size, write_size);
    return false;
  }
  bool ok = idx_appender_.Put(entry_id, start, size);
  if (!ok) {
    LOG(WARNING, "fail to add data idx for segment %s", segment_name_.c_str());
    return false;
  }
  if (idx != NULL) {
    idx.start = start;
    idx.size = size;
  }
  data_appender_.Flush();
  consumed = ::baidu::common::timer::get_micros() - consumed;
  LOG(DEBUG, "write data to segment %s consumed %lld",
      segment_name_.c_str(), consumed);
  return true;
}



bool SegmentAppender::Sync() {
  bool ok = data_appender_.Sync();
  if (!ok) {
    LOG(WARNING, "fail to fsync segment %s for %s",
        segment_name_.c_str(), strerror(errno));
    return false;
  }
  LOG(DEBUG, "sync segment file %s successfully", segment_name_.c_str());
  return true;
}

void SegmentAppender::Close() {
  data_appender_.Close();
  idx_appender_.Close();
}

}
