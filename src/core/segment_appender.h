#ifndef EL_CORE_SEGMENT_APPENDER_H_
#define EL_CORE_SEGMENT_APPENDER_H_

#include <stdio>
#include <string>
#include <stdint.h>
#include "mutex.h"
#include "segment_codec.h"

namespace el {

class SegmentAppender {

public:
  SegmentAppender(const std::string& folder, 
                 const std::string& filename,
                 uint64_t max_size);

  ~SegmentAppender();

  bool Init();

  bool Appendable();
  //
  bool Append(const char* data, uint64_t size, uint64_t offset);

  bool Sync();

private:
  std::string folder_;
  std::string filename_;
  uint64_t max_size_;

  uint64_t current_size_;
  FILE* fd_;
  ::baidu::common::Mutex mu_;
  SegmentCodec codec_;
};

}
#endif
