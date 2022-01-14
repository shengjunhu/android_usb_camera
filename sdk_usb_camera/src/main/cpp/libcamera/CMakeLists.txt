cmake_minimum_required(VERSION 3.10.2)

# set config
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# main
include_directories(../Common.h ../libyuv/include ../libjpeg-turbo)
add_library(${MODULE_CORE} STATIC Preview.cpp Decoder.cpp UsbMessenger.cpp UsbCamera.cpp)
target_include_directories(${MODULE_CORE} PRIVATE ../Common.h ../libyuv/include ../libjpeg-turbo)

# copy out the lib binary
set_target_properties(${MODULE_CORE} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DIR_OUT}/${ANDROID_ABI}")
