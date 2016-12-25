#ifndef EL_CORE_SEGMENT_APPENDER_H_
#define EL_CORE_SEGMENT_APPENDER_H_

#include <stdint.h>
#include "mutex.h"
#include "segment_codec.h"
#include "base_appender.h"
#include "index_appender.h"

namespace el {

class SegmentAppender {

public:
  SegmentAppender(const std::string& folder, 
                  const std::string& segment_name,
                  const std::string& index_name,
                  uint64_t segment_max_size,
                  uint64_t index_max_size);

  ~SegmentAppender();

  bool Init();

  // append data to segment with  entry id 
  bool Append(const char* data, uint64_t size, uint64_t entry_id);

  bool Append(const char* data, 
      uint64_t size, 
      uint64_t entry_id,
      EntryIndex* idx);

  bool Sync();

  void Close();

private:
  std::string segment_name_;
  SegmentCodec codec_;
  BaseAppender data_appender_;
  IndexAppender idx_appender_;
};

}
#endif
/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
