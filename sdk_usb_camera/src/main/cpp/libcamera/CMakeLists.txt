cmake_minimum_required(VERSION 3.10.2)

# set config
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# link directories
link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../libjpeg-turbo
        ${CMAKE_CURRENT_SOURCE_DIR}/../libyuv/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb
        ${CMAKE_CURRENT_SOURCE_DIR}/../libuvc/include)

# include directories
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../libjpeg-turbo
        ${CMAKE_CURRENT_SOURCE_DIR}/../libyuv/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb
        ${CMAKE_CURRENT_SOURCE_DIR}/../libuvc/include)

add_library(${MODULE_CORE} STATIC
        UsbCamera.cpp
        UsbMessenger.cpp
        FrameProcess.cpp
        FrameDecoder.cpp
        FrameEncoder.cpp
        FramePreview.cpp)

# target link libraries
target_link_libraries(
        ${MODULE_CORE}
        android
        ${MODULE_TURBO}
        ${MODULE_YUV}
        ${MODULE_UVC})