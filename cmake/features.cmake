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

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(BUILD_DEBUG true)
    add_definitions(-DDEBUG)
else()
    add_definitions(-DNDEBUG)
endif()
