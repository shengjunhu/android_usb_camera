cmake_minimum_required(VERSION 3.10.2)

# set flag
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# generate target file
set(CMAKE_VERBOSE_MAKEFILE ON)
set(MAIN uvc)

# option
option(ENABLE_UVC_DEBUGGING "Enable UVC debugging" OFF)

option(LOG_SWITCH "option for print log" ON)
if(LOG_SWITCH)
  add_definitions(-DLOG_SWITCH)
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

# main
add_library(${MAIN} STATIC ${DIR_SRC})
target_include_directories(${MAIN} PRIVATE include)

# copy out the lib binary
set(DIR_OUT ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
set_target_properties(${MAIN} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DIR_OUT}/${ANDROID_ABI}")
