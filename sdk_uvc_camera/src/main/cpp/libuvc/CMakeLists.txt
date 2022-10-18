cmake_minimum_required(VERSION 3.10.2)
project(libuvc
  VERSION 0.0.6
  LANGUAGES C
)

# set source
set(DIR_SRC
        src/ctrl.c
        src/ctrl-gen.c
        src/device.c
        src/diag.c
        src/frame.c
        src/init.c
        src/stream.c
        src/misc.c)

# set option for log
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
  message(STATUS "UVC DEBUGGING Enable!")
  add_definitions(-DUVC_DEBUGGING)
endif ()

# set option for jpeg
option(HAVE_JPEG "Enable decode mjpeg" OFF)
if(HAVE_JPEG)
  set(LIBUVC_HAS_JPEG TRUE)
  set(PKGCONFIG_JPEG_LDFLAG "-ljpeg")
  list(APPEND DIR_SRC src/frame-mjpeg.c)
endif()

# set link directories
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb)

# set include
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb)

# set module
add_library(${MODULE_UVC} STATIC ${DIR_SRC})

# set target link libraries
target_link_libraries(${MODULE_UVC} ${MODULE_USB})