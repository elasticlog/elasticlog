/*
 * test_segment_appender.cc
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#include "segment_appender.h"
#include "segment_reader.h"

#include "gtest/gtest.h"
#include "logging.h"

namespace el{

class SA_Test : public ::testing::Test {

public:
  SA_Test() {}
  ~SA_Test() {}

};

TEST_F(SA_Test, Init) {
  SegmentAppender appender("/tmp", "0.segm", "0.idx", 1024 * 1024, 1024 * 1024);
  SegmentReader reader("/tmp", "0.segm");
  bool ok =false;
  ok = appender.Init();
  ASSERT_EQ(true, ok);
  std::string data ="hello elasticlog xxxxxxxxxxxxxxxx!";
  uint64_t offset  = 64;
  EntryIndex* idx = new EntryIndex();
  ok = appender.Append(data.c_str(), data.size(), offset, idx);
  ASSERT_EQ(true, ok);
  reader.PutIdxCache(offset, idx);
  /*ok = appender.Sync();
  ASSERT_EQ(true, ok);
  appender.Close();*/
  uint32_t sid = 0;
  ok = reader.NewScope(&sid);
  ASSERT_EQ(true, ok);
  LogItem item;
  ok = reader.Next(sid, &item);
  ASSERT_EQ(true, ok);
  ASSERT_EQ(item.data, data);
  EntryIndex* idx2 = new EntryIndex();
  ok = appender.Append(data.c_str(), data.size(), offset+1, idx2);
  ASSERT_EQ(true, ok);
  reader.PutIdxCache(offset+1, idx2);
  LogItem item1;
  ok = reader.Next(sid, &item1);
  ASSERT_EQ(true, ok);
  ASSERT_EQ(item1.data, data);
}

}

int main(int argc, char** argv) {
  ::baidu::common::SetLogLevel(::baidu::common::DEBUG);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
