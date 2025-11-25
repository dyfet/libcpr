# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(FindPkgConfig)

if(WIN32)
    if(MWSVC)
            message(FATAL_ERROR "MSVC not supported; use MingW32")
    endif()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

if(WIN32 OR MSYS)
    list(APPEND EXTRA_LINK_LIBRARIES ws2_32 iphlpapi)
    set(LIBCPR_EXTRA_LIBRARIES "${MICROBE_EXTRA_LIBRARIES} -lws2_32 -liphlpapi")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBCPR_EXTRA_FLAGS}")

add_compile_options(
    -ffunction-sections
    -fdata-sections
)

add_link_options(
    -Wl,--no-whole-archive
    -Wl,--gc-sections
    -Wl,--gc-sections
    -Wl,--as-needed
    -Wl,--discard-all
    -Wl,--no-undefined
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(BUILD_DEBUG true)
    add_definitions(-DDEBUG)
else()
    add_definitions(-DNDEBUG)
endif()
