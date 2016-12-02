SET(CORE_SRC segment_appender.cc)
SET(CORE_HEADER segment_appender.h segment_codec.h)
ADD_LIBRARY(el_core STATIC ${CORE_SRC} ${CORE_HEADER})
