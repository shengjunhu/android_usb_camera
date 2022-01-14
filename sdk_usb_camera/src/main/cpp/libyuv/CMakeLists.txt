cmake_minimum_required(VERSION 3.10.2)

# set flag
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# generate target file
set(CMAKE_VERBOSE_MAKEFILE ON)
set(MAIN yuv)

# log switch
option(LOG_SWITCH "option for print log" ON)
if(LOG_SWITCH)
  add_definitions(-DLOG_SWITCH)
endif(LOG_SWITCH)

# set source
set(DIR_SRC source/compare.cc
            source/compare_common.cc
            source/convert.cc
            source/convert_argb.cc
            source/convert_from.cc
            source/convert_from_argb.cc
            source/convert_to_argb.cc
            source/convert_to_i420.cc
            source/cpu_id.cc
            source/planar_functions.cc
            source/rotate.cc
            source/rotate_any.cc
            source/rotate_argb.cc
            source/rotate_common.cc
            source/row_any.cc
            source/row_common.cc
            source/scale.cc
            source/scale_any.cc
            source/scale_argb.cc
            source/scale_common.cc
            source/video_common.cc)

if(${ANDROID_ABI} MATCHES "armeabi-v7a")
  #add_definitions(-DHAVE_JPEG)
  add_definitions(-DLIBYUV_NEON)
  set(DIR_ABI source/compare_neon.cc
              source/rotate_neon.cc
              source/scale_neon.cc
              source/row_neon.cc)
elseif(${ANDROID_ABI} MATCHES "arm64-v8a")
  #add_definitions(-DHAVE_JPEG)
  add_definitions(-DLIBYUV_NEON)
  set(DIR_ABI source/compare_neon64.cc
              source/rotate_neon64.cc
              source/scale_neon64.cc
              source/row_neon64.cc)
endif()

# main
list(APPEND DIR_SRC ${DIR_ABI})
add_library(${MAIN} STATIC ${DIR_SRC})
target_include_directories(${MAIN} PRIVATE include)

# copy out the lib binary
set(DIR_OUT ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
set_target_properties(${MAIN} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DIR_OUT}/${ANDROID_ABI}")
