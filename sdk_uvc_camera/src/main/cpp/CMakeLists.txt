cmake_minimum_required(VERSION 3.10.2)
project(sdk_uvc_camera LANGUAGES CXX C)

# set config
#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused")

# module turbojpeg
set(MODULE_TURBO turbojpeg-static)
add_subdirectory(libjpeg-turbo)

# module yuv
set(MODULE_YUV yuv)
add_subdirectory(libyuv)

# module usb
set(MODULE_USB usb)
add_subdirectory(libusb)

# module uvc
set(MODULE_UVC uvc)
add_subdirectory(libuvc)

# module main
set(MODULE_MAIN camera)
add_subdirectory(libcamera)