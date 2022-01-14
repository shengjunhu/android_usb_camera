cmake_minimum_required(VERSION 3.10.2)

# set config
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# option
option(ENABLE_UVC_DEBUGGING "Enable UVC debugging" OFF)

# set source
set(DIR_SRC src/ctrl.c
        src/ctrl-gen.c
        src/device.c
        src/diag.c
        src/frame.c
        src/init.c
        src/stream.c
        src/misc.c)

option(HAVE_JPEG "Enable decode mjpeg" ON)
if(HAVE_JPEG)
  set(LIBUVC_HAS_JPEG TRUE)
  set(PKGCONFIG_JPEG_LDFLAG "-ljpeg")
  list(APPEND DIR_SRC src/frame-mjpeg.c)
endif()

# main
add_library(${MODULE_UVC} STATIC ${DIR_SRC})
target_include_directories(${MODULE_UVC} PRIVATE include)

# copy out the lib binary
set_target_properties(${MODULE_UVC} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${DIR_OUT}/${ANDROID_ABI}")
