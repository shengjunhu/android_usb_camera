cmake_minimum_required(VERSION 3.10.2)

# set config
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

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
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/android
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os)
add_library(${MODULE_USB} STATIC ${DIR_SRC})

# find system support library
find_library(log-lib log)

# target link libraries
target_link_libraries(${MODULE_USB} ${log-lib})
