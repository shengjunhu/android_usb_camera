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
if(${SWITCH_LOG})
    add_definitions(-DSWITCH_LOG)
endif()

# set global variable
set(MODULE_TURBO turbo)
set(MODULE_YUV   yuv)
set(MODULE_USB   usb)
set(MODULE_UVC   uvc)
set(MODULE_CORE  core)
set(MODULE_MAIN  camera)
set(DIR_OUT      ${CMAKE_CURRENT_SOURCE_DIR}/obj)

# module turbo
add_subdirectory(libjpeg-turbo)
# module yuv
add_subdirectory(libyuv)
# module usb
add_subdirectory(libusb)
# module uvc
add_subdirectory(libuvc)
# module camera
add_subdirectory(libcamera)

#[[
# link libraries: libturbo
add_library(${MODULE_TURBO} STATIC IMPORTED)
set_target_properties(${MODULE_TURBO} PROPERTIES IMPORTED_LOCATION ${DIR_OUT}/${ANDROID_ABI}/libturbo.a)

# link libraries: libyuv
add_library(${MODULE_YUV} STATIC IMPORTED)
set_target_properties(${MODULE_YUV} PROPERTIES IMPORTED_LOCATION ${DIR_OUT}/${ANDROID_ABI}/libyuv.a)

# link libraries: libusb
add_library(${MODULE_USB} STATIC IMPORTED)
set_target_properties(${MODULE_USB} PROPERTIES IMPORTED_LOCATION ${DIR_OUT}/${ANDROID_ABI}/libusb.a)

# link libraries: libuvc
add_library(${MODULE_UVC} STATIC IMPORTED)
set_target_properties(${MODULE_UVC} PROPERTIES IMPORTED_LOCATION ${DIR_OUT}/${ANDROID_ABI}/libuvc.a)

# link libraries: libcamera
add_library(${MODULE_CORE} STATIC IMPORTED)
set_target_properties(${MODULE_CORE} PROPERTIES IMPORTED_LOCATION ${DIR_OUT}/${ANDROID_ABI}/libcore.a)
]]

# moudle main
include_directories(Common.h libcamera)
add_library(${MODULE_MAIN} SHARED Common.cpp OnLoad.cpp)

# find system support library
find_library(log-lib log)

# target link libraries
target_link_libraries(${MODULE_MAIN}
                      ${log-lib}
                      android
                      ${MODULE_TURBO}
                      ${MODULE_YUV}
                      ${MODULE_USB}
                      ${MODULE_UVC}
                      ${MODULE_CORE})