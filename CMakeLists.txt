# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

cmake_minimum_required(VERSION 3.16.0)
Project(libcpr VERSION 0.6.0 LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(C_STANDARD_REQUIRED 17)

file(
    GLOB LINT_SOURCES
    RELATIVE ${PROJECT_SOURCE_DIR}
    src/*.h src/*.c)

include(cmake/custom.cmake OPTIONAL)
include(cmake/project.cmake)
include(cmake/features.cmake)
include(cmake/coverage.cmake)
include(cmake/clint.cmake)
include(cmake/deploy.cmake OPTIONAL)

file(GLOB optional .git[a-z]* *.json *.in *.yml .clang* cmake/*)
file(GLOB headers src/*.h)
file(GLOB sources src/*.c)
file(GLOB markdown *.md)

configure_file(libcpr.pc.in libcpr.pc)
enable_testing()

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
add_library(cpr ${sources} ${headers})
if(BUILD_SHARED_LIBS AND CMAKE_BUILD_TYPE MATCHES "^Rel")
    set_target_properties(cpr PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    target_compile_options(cpr PUBLIC -pthread)
    target_link_options(cpr PUBLIC -pthread)
endif()

# testing

add_executable(test_tty test/tty.c src/tty.h)
add_test(NAME test-tty COMMAND test_tty)
target_link_libraries(test_tty PRIVATE cpr)

add_executable(test_memory test/memory.c src/memory.h)
add_test(NAME test-memory COMMAND test_memory)
target_link_libraries(test_memory PRIVATE cpr)

add_executable(test_memio test/memio.c src/memio.h)
add_test(NAME test-memio COMMAND test_memio)
target_link_libraries(test_memio PRIVATE cpr)

add_executable(test_mempager test/mempager.c src/mempager.h)
add_test(NAME test-mempager COMMAND test_mempager)
target_link_libraries(test_mempager PRIVATE cpr)

add_executable(test_system test/system.c src/system.h)
add_test(NAME test-system COMMAND test_system)
target_link_libraries(test_system PRIVATE cpr)

add_executable(test_string test/string.c src/string.h)
add_test(NAME test-string COMMAND test_string)
target_link_libraries(test_string PRIVATE cpr)

add_executable(test_endian test/endian.c src/endian.h)
add_test(NAME test-endian COMMAND test_endian)

add_executable(test_socket test/socket.c src/socket.h)
add_test(NAME test-socket COMMAND test_socket)

add_executable(test_clock test/clock.c src/clock.h)
add_test(NAME test-clock COMMAND test_clock)

add_executable(test_thread test/thread.c src/thread.h)
add_test(NAME test-thread COMMAND test_thread)
target_link_libraries(test_thread PRIVATE cpr)

add_executable(test_keyfile test/keyfile.c src/keyfile.h)
set_target_properties(test_keyfile PROPERTIES COMPILE_DEFINITIONS "TEST_DATA=\"${CMAKE_SOURCE_DIR}/test\"")
target_link_libraries(test_keyfile PRIVATE cpr)
add_test(NAME test-keyfile COMMAND test_keyfile)

# Extras...
add_custom_target(header-files SOURCES ${headers})
add_custom_target(support-files SOURCES ${markdown} ${optional})

install(FILES ${headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cpr")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcpr.pc DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")

install(TARGETS cpr
    EXPORT cprTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpr
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cpr
)

install(EXPORT cprTargets
    FILE libcprTargets.cmake
    NAMESPACE libcpr::
    DESTINATION lib/cmake/libcpr
)

