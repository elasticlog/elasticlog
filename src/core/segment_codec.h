#ifndef EL_CORE_SEGMENT_CODEC_H_
#define EL_CORE_SEGMENT_CODEC_H_

#include <string>
#include <string.h>
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
  SegmentCodec() {}
  ~SegmentCodec() {}

  bool Encode(const SegmentHeader& header, std::string* buf) {
    uint64_t entry_size = sizeof(uint64_t) * 2 + 10 + header.data_size;
    buf->resize(entry_size);
    char* rbuffer = reinterpret_cast<char*>(& ((*buf)[0]));
    memcpy(rbuffer, static_cast<const void*>(MAGIC_KEY), 10);
    rbuffer += 10;
    memcpy(rbuffer, static_cast<const void*>(&header.offset), 8);
    rbuffer += 8;
    memcpy(rbuffer, static_cast<const void*>(&header.data_size), 8);
    return true;
  }

  bool Decode(const std::string* buf, SegmentHeader* header) {
    const char* rbuffer = buf->data();
    std::string magic_key;
    magic_key.resize(10);
    char* magic_key_buf = reinterpret_cast<char*>(&(magic_key[0]));
    memcpy(magic_key_buf, rbuffer, 10);
    rbuffer += 10;
    memcpy(static_cast<void*>(&header->offset), rbuffer, 8);
    rbuffer += 8;
    memcpy(static_cast<void*>(&header->data_size), rbuffer, 8);
    return true;
  }
};


}
#endif
