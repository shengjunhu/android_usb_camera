cmake_minimum_required(VERSION 3.10.2)
project("sdk_uvc_camera2")

# set flag
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# set common name
set(DIR_TURBO   libjpeg-turbo)
set(DIR_CAMERA  libcamera)
set(DIR_USB     libusb)
set(DIR_UVC     libuvc)
set(DIR_YUV     libyuv)
set(MAIN        camera)

# log switch
option(LOG_SWITCH "option for print log" ON)
if(LOG_SWITCH)
    add_definitions(-DLOG_SWITCH)
endif(LOG_SWITCH)

# module camera
#add_subdirectory(${DIR_CAMERA})
# module turbo
add_subdirectory(${DIR_TURBO})
# module usb
# add_subdirectory(${DIR_USB})
# module uvc
# add_subdirectory(${DIR_UVC})
# module yuv
# add_subdirectory(${DIR_YUV})

# link libraries
#set(DIR_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI})
# libcamera
#add_library(core STATIC IMPORTED)
#set_target_properties(core PROPERTIES IMPORTED_LOCATION ${DIR_LIB}/libcore.a)
# libturbo
#add_library(turbo STATIC IMPORTED)
#set_target_properties(turbo PROPERTIES IMPORTED_LOCATION ${DIR_LIB}/libturbo.a)
# libusb
#add_library(usb STATIC IMPORTED)
#set_target_properties(usb PROPERTIES IMPORTED_LOCATION ${DIR_LIB}/libusb.a)
# libuvc
#add_library(uvc STATIC IMPORTED)
#set_target_properties(uvc PROPERTIES IMPORTED_LOCATION ${DIR_LIB}/libuvc.a)
# libyuv
#add_library(yuv STATIC IMPORTED)
#set_target_properties(yuv PROPERTIES IMPORTED_LOCATION ${DIR_LIB}/libyuv.a)

# moudle main
include_directories(Common.h ${CMAKE_CURRENT_SOURCE_DIR}/${DIR_CAMERA})
add_library(${MAIN} SHARED Common.cpp OnLoad.cpp)

# find system support library
find_library(log-lib log)

# target link libraries
#target_link_libraries(${MAIN} ${log-lib} android core turbo usb uvc yuv)
target_link_libraries(${MAIN} ${log-lib} android #[[turbo]])