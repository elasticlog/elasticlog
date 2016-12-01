PROJECT(elasticlog)
INCLUDE_DIRECTORIES(/usr/include /usr/local/include ${PROJECT_SOURCE_DIR}/thirdparty/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/thirdparty/lib /usr/lib /usr/local/lib)
ADD_SUBDIRECTORY(src)
