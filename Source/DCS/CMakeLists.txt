cmake_minimum_required(VERSION 2.4)
project(DCS)

# Default to a release build
if(NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT MSVC AND NOT CMAKE_BUILD_TYPE)

# Various definitions for all builds
if(UNIX)
  add_definitions(-DUNIX)
  add_definitions(-DLinux=1)
  add_definitions(-fPIC)
endif(UNIX)

if(MSVC)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif(MSVC)

# Test system endianness
include(TestBigEndian)
test_big_endian(VSI_BIG_ENDIAN)
if(VSI_BIG_ENDIAN)
  add_definitions(-DSIM_BIG_ENDIAN)
else(VSI_BIG_ENDIAN)
  add_definitions(-DSIM_LITTLE_ENDIAN)
endif(VSI_BIG_ENDIAN)

add_subdirectory(STD)
add_subdirectory(NET)
add_subdirectory(DCS2)

