/*
 * test_segment_appender.cc
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#include "segment_appender.h"

#include "gtest/gtest.h"

namespace el{

class SA_Test : public ::testing::Test {

public:
  SA_Test() {}
  ~SA_Test() {}

};

TEST_F(SA_Test, Init) {
  SegmentAppender appender("/tmp", "0.segm", 1024 * 1024);
  bool ok = appender.Init();
  ASSERT_EQ(true, ok);
  std::string data ="hello world!";
  uint64_t offset  = 0;
  ok = appender.Append(data.c_str(), data.size(), offset);
  ASSERT_EQ(true, ok);
  ok = appender.Sync();
  ASSERT_EQ(true, ok);
  appender.Close();
}

}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
