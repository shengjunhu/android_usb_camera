cmake_minimum_required(VERSION 3.10.2)

# set config
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# option
option(ENABLE_UVC_DEBUGGING "Enable UVC debugging" ON)
if(ENABLE_UVC_DEBUGGING)
  add_definitions(-DUVC_DEBUGGING)
endif()

# set source
set(DIR_SRC src/ctrl.c
        src/ctrl-gen.c
        src/device.c
        src/diag.c
        src/frame.c
        src/init.c
        src/stream.c
        src/misc.c)

option(HAVE_JPEG "Enable decode mjpeg" OFF)
if(HAVE_JPEG)
  set(LIBUVC_HAS_JPEG TRUE)
  set(PKGCONFIG_JPEG_LDFLAG "-ljpeg")
  list(APPEND DIR_SRC src/frame-mjpeg.c)
endif()

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb)

add_library(${MODULE_UVC} STATIC ${DIR_SRC})

# target link libraries
target_link_libraries(${MODULE_UVC} ${MODULE_USB})