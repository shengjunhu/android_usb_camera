cmake_minimum_required(VERSION 3.10.2)

# set config
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

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
add_library(${MODULE_USB} STATIC ${DIR_SRC})
target_include_directories(${MODULE_USB} PRIVATE android libusb libusb/os)

# copy out the lib binary
set_target_properties(${MODULE_USB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DIR_OUT}/${ANDROID_ABI}")
