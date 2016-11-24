#ifndef EL_CORE_SEGMENT_CODEC_H_
#define EL_CORE_SEGMENT_CODEC_H_

#include <string>
#include <stdlib.h>
#include <stdio.h>

namespace el{

const static char* MAGIC_KEY = "EL_SEGMENT";

struct SegmentHeader {
  uint64_t data_size;
  uint64_t offset;
};

class SegmentCodec {

public:
  SegmentCodec();
  ~SegmentCodec();

  bool Encode(const SegmentHeader& header, std::string* buf) {
    uint64_t total_size = sizeof(uint64_t) * 2;
    buf->resize(total_size + 10);
    char* rbuffer = reinterpret_cast<char*>(& ((*buf)[0]));
    memcpy(rbuffer, static_cast<const void*>(MAGIC_KEY), 10);
    rbuffer += 10;
    memcpy(rbuffer, static_cast<const void*>(total_size), 8);
    rbuffer += 8;
    memcpy(rbuffer, static_cast<const void*>(header), 8);
  }

  bool Decode(const std::string* buf, SegmentHeader* header);
};


}
#endif
