INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} 
  ${PROJECT_SOURCE_DIR}/src 
  ${PROJECT_SOURCE_DIR}/src/proto 
  ${PROJECT_SOURCE_DIR}/src/common
  ${PROJECT_SOURCE_DIR}/src/rpc
  ${PROJECT_SOURCE_DIR}/src/core)
ADD_LIBRARY(ellet_impl STATIC ellet_impl.cc file_utils.cc ellet_impl.h)
ADD_EXECUTABLE(ellet ellet_main.cc)
TARGET_LINK_LIBRARIES(ellet ellet_impl el_core el_flags ellet_pb sofa-pbrpc gtest common pthread z snappy gflags rt)

ADD_EXECUTABLE(ellet_test test_ellet_impl.cc)
TARGET_LINK_LIBRARIES(ellet_test ellet_pb protobuf sofa-pbrpc gtest el_flags common pthread rt z snappy gflags)
