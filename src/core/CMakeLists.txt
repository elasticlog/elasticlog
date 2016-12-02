SET(CORE_SRC segment_appender.cc)
SET(CORE_HEADER segment_appender.h segment_codec.h)
ADD_LIBRARY(el_core STATIC ${CORE_SRC} ${CORE_HEADER})

ADD_EXECUTABLE(appender_test test_segment_appender.cc)
TARGET_LINK_LIBRARIES(appender_test el_core gtest common pthread)

