INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} 
  ${PROJECT_SOURCE_DIR}/src 
  ${PROJECT_SOURCE_DIR}/src/proto 
  ${PROJECT_SOURCE_DIR}/src/common
  ${PROJECT_SOURCE_DIR}/src/rpc
  ${PROJECT_SOURCE_DIR}/src/core)
ADD_LIBRARY(master_impl STATIC master_impl.cc)
ADD_EXECUTABLE(elmaster master_main.cc)
TARGET_LINK_LIBRARIES(elmaster master_impl el_core el_flags elmaster_pb sofa-pbrpc gtest common pthread z snappy gflags rt)

