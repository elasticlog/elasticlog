/*
 * test_ellet_impl.cc
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ellet.pb.h"

#include "gtest/gtest.h"
#include "gflags/gflags.h"
#include "logging.h"
#include "rpc_client.h"
#include "time.h"

using ::baidu::common::INFO;
using ::baidu::common::DEBUG;
using ::baidu::common::WARNING;

DECLARE_string(ellet_endpoint);

namespace el {

class ElLet_Test : public ::testing::Test {

public:
  ElLet_Test() {}
  ~ElLet_Test() {}
};

TEST_F(ElLet_Test, DeploySegment) {
  RpcClient* client = new RpcClient();
  ElLet_Stub* ellet = NULL;
  bool ok = client->GetStub(FLAGS_ellet_endpoint, &ellet);
  ASSERT_EQ(true, ok);
  DeploySegmentRequest request;
  request.set_log_id(10);
  request.set_log_name("test");
  request.set_partion_id(1);
  request.set_primary_endpoint("local");
  request.set_state(kPaused);
  request.add_segment_ids(10);
  request.add_segment_ids(11);
  request.set_segment_max_size(1024 * 1024 * 5);
  DeploySegmentResponse response;
  ok = client->SendRequest(ellet, &ElLet_Stub::DeploySegment,
      &request,&response, 5, 1);
  ASSERT_EQ(true, ok);
  LOG(INFO, "start sync write 10M to disk");
  uint64_t consumed = ::baidu::common::timer::get_micros();
  for (int i = 0; i < 500000; i++) {
    int64_t i_con = ::baidu::common::timer::get_micros();
    AppendEntryRequest areq;
    LogEntry* entry = areq.mutable_entry();
    entry->set_entry_id(1);
    entry->set_log_id(10);
    entry->set_partion_id(1);
    entry->set_content("hello");
    AppendEntryResponse arep;
    ok = client->SendRequest(ellet, &ElLet_Stub::AppendEntry,
              &areq, &arep, 5, 1);
    ASSERT_EQ(true, ok);
    LOG(INFO, "consumed %lld", ::baidu::common::timer::get_micros() - i_con);
  }
  consumed = ::baidu::common::timer::get_micros() - consumed;
  LOG(INFO, "write 10M completed with time %lld", consumed);
  delete ellet;
  delete client;
}

}

int main(int argc, char** argv) {
  ::baidu::common::SetLogLevel(::baidu::common::DEBUG);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
