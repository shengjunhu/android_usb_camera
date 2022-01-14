cmake_minimum_required(VERSION 3.10.2)

# set flag
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# generate target file
set(CMAKE_VERBOSE_MAKEFILE ON)
set(MAIN core)

# log switch
option(LOG_SWITCH "option for print log" ON)
if(LOG_SWITCH)
  add_definitions(-DLOG_SWITCH)
endif(LOG_SWITCH)

# main
add_library(${MAIN} STATIC Decoder.cpp Preview.cpp)
target_include_directories(${MAIN} PRIVATE Decoder.h UsbCamera.h UsbMessenger.h Preview.h)

# copy out the lib binary
set(DIR_OUT ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
set_target_properties(${MAIN} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DIR_OUT}/${ANDROID_ABI}")
