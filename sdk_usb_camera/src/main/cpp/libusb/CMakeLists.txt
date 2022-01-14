cmake_minimum_required(VERSION 3.10.2)

# set flag
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(MAIN usb)

# option
option(ENABLE_UVC_DEBUGGING "Enable UVC debugging" OFF)

option(LOG_SWITCH "option for print log" ON)
if(LOG_SWITCH)
  add_definitions(-DLOG_SWITCH)
endif()

# set source
set(DIR_SRC libusb/core.c
            libusb/descriptor.c
            libusb/hotplug.c
            libusb/io.c
            libusb/sync.c
            libusb/strerror.c
            libusb/os/linux_usbfs.c
            libusb/os/events_posix.c
            libusb/os/threads_posix.c
            libusb/os/linux_netlink.c)

# main
add_library(${MAIN} STATIC ${DIR_SRC})
target_include_directories(${MAIN} PRIVATE android libusb libusb/os)

# copy out the lib binary
set(DIR_OUT ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
set_target_properties(${MAIN} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DIR_OUT}/${ANDROID_ABI}")
