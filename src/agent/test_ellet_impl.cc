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
  request.set_segment_max_size(1024 * 1024 * 128);
  DeploySegmentResponse response;
  ok = client->SendRequest(ellet, &ElLet_Stub::DeploySegment,
      &request,&response, 5, 1);
  ASSERT_EQ(true, ok);
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
