cmake_minimum_required(VERSION 3.10.2)
project("sdk_usb_camera")

# set config
#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused")

# log level
add_definitions(-DLOG_E)
add_definitions(-DLOG_W)
if(${CMAKE_BUILD_TYPE} MATCHES debug OR
   ${CMAKE_BUILD_TYPE} MATCHES Debug OR
   ${CMAKE_BUILD_TYPE} MATCHES DEBUG)
    add_definitions(-DLOG_I)
    add_definitions(-DLOG_D)
endif ()

# set global variable
set(MODULE_TURBO turbojpeg-static)
set(MODULE_YUV   yuv)
set(MODULE_USB   usb)
set(MODULE_UVC   uvc)
set(MODULE_CORE  core)
set(MODULE_MAIN  camera)

# module turbo
add_subdirectory(libjpeg-turbo)
# module yuv
add_subdirectory(libyuv)
# module usb
add_subdirectory(libusb)
# module uvc
add_subdirectory(libuvc)
# module core
add_subdirectory(libcamera)

# link directories
link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo
        ${CMAKE_CURRENT_SOURCE_DIR}/libuvc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libcamera)

# include directories
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo
        ${CMAKE_CURRENT_SOURCE_DIR}/libuvc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libcamera)

add_library(${MODULE_MAIN} SHARED Common.cpp OnLoad.cpp)

# find system support library
find_library(log-lib log)

# target link libraries
target_link_libraries(
        ${MODULE_MAIN}
        ${log-lib}
        ${MODULE_CORE})
