cmake_minimum_required(VERSION 3.10.2)
project("sdk_usb_camera")

# set config
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# switch log
message(STATUS "Print log ${SWITCH_LOG}")
if (${SWITCH_LOG})
    add_definitions(-DSWITCH_LOG)
endif ()

# set global variable
set(MODULE_YUV   yuv)
set(MODULE_USB   usb)
set(MODULE_UVC   uvc)
set(MODULE_CORE  core)
set(MODULE_MAIN  camera)
set(MODULE_TURBO turbojpeg-static)

# module usb
add_subdirectory(libusb)
# module uvc
add_subdirectory(libuvc)
# module yuv
add_subdirectory(libyuv)
# module turbo
add_subdirectory(libjpeg-turbo)
# module core
add_subdirectory(libcamera)

# moudle main
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/Common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libcamera)
add_library(${MODULE_MAIN} SHARED Common.cpp OnLoad.cpp)

# find system support library
find_library(log-lib log)

# target link libraries
target_link_libraries(
        ${MODULE_MAIN}
        ${log-lib}
        android
        ${MODULE_USB}
        ${MODULE_UVC}
        ${MODULE_YUV}
        ${MODULE_TURBO}
        ${MODULE_CORE})
