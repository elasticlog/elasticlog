#ifndef EL_CORE_SEGMENT_APPENDER_H_
#define EL_CORE_SEGMENT_APPENDER_H_

#include <stdint.h>
#include "mutex.h"
#include "segment_codec.h"
#include "base_appender.h"

namespace el {

class SegmentAppender {

public:
  SegmentAppender(const std::string& folder, 
                 const std::string& filename,
                 uint64_t max_size);

  ~SegmentAppender();

  bool Init();

  bool Append(const char* data, uint64_t size, uint64_t offset);

  bool Sync();

  void Close();
private:

  SegmentCodec codec_;
  BaseAppender appender_;
};

}
#endif
/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
