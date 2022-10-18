cmake_minimum_required(VERSION 3.10.2)
project(libusb
  VERSION 1.0.26
  LANGUAGES C
)

# set config
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -pthread")

# set option for log
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    message(STATUS "USB DEBUGGING Enable!")
    add_definitions(-DENABLE_DEBUG_LOGGING)
endif ()

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

# set include
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/android
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os)

# set module
add_library(${MODULE_USB} STATIC ${DIR_SRC})

# set target
target_link_libraries(${MODULE_USB})
