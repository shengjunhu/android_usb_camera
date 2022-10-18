cmake_minimum_required(VERSION 3.10.2)

# set config
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused -frtti -fexceptions")

# set option for log
add_definitions(-DLOG_E)
add_definitions(-DLOG_W)
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_definitions(-DLOG_I)
    add_definitions(-DLOG_D)
endif ()

# link directories
link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../libjpeg-turbo
        ${CMAKE_CURRENT_SOURCE_DIR}/../libyuv/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb
        ${CMAKE_CURRENT_SOURCE_DIR}/../libuvc/include)

# include directories
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../libjpeg-turbo
        ${CMAKE_CURRENT_SOURCE_DIR}/../libyuv/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb
        ${CMAKE_CURRENT_SOURCE_DIR}/../libuvc/include)

# set main module
add_library(${MODULE_MAIN} SHARED
        frame_converter.cpp
        frame_decoder.cpp
        frame_encoder.cpp
        frame_preview.cpp
        frame_preview_jni.cpp
        uvc_camera.cpp
        uvc_camera_jni.cpp)

# find system support library
find_library(log-lib log)

# target link libraries
target_link_libraries(
        ${MODULE_MAIN}
        ${MODULE_TURBO}
        ${MODULE_YUV}
        ${MODULE_UVC}
        ${log-lib})