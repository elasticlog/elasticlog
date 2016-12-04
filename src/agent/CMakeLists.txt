INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/proto)
ADD_LIBRARY(ellet_impl STATIC ellet_impl.cc ellet_impl.h)
ADD_EXECUTABLE(ellet ellet_main.cc)
TARGET_LINK_LIBRARIES(ellet ellet_impl el_core el_flags ellet_pb sofa-pbrpc gtest common pthread z snappy gflags)
