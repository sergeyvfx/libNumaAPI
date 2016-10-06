# Copyright (c) 2016, libnumaapi authors
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#
# Author: Sergey Sharybin (sergey.vfx@gmail.com)

project(libnumaapi)

cmake_minimum_required (VERSION 2.6)
if (NOT CMAKE_VERSION VERSION_LESS 2.8.4)
    cmake_policy (SET CMP0017 NEW)
endif()

include(CMakeParseArguments)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
message(STATUS "Project source dir = ${PROJECT_SOURCE_DIR}")
message(STATUS "Project build dir = ${CMAKE_BINARY_DIR}")

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

option(WITH_EXAMPLES "Build the examples programs" ON)
option(WITH_DYNLOAD "Use dynamic loading instead of direct linking" OFF)

if(WITH_DYNLOAD)
  add_definitions(-DWITH_DYNLOAD)
endif()

include_directories(include)

add_library(libnumaapi
  source/libnumaapi.c
  source/libnumaapi_linux.c

  include/libnumaapi.h
)

if(UNIX AND NOT APPLE)
  if(WITH_DYNLOAD)
    target_link_libraries(libnumaapi ${CMAKE_DL_LIBS})
  else()
    # TODO(sergey): Replace this with find_package()'d library.
    target_link_libraries(libnumaapi numa)
  endif()
endif()

if(WITH_EXAMPLES)
  add_executable(numa_example example/example.c)
  target_link_libraries(numa_example libnumaapi)
endif()
