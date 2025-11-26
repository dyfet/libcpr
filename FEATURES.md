# Features

Libcpr consists of a series of individual and roughly stand-alone C header
files with inline static functions and simple C pre-processor macros that
typically are installed under and included from /usr/include/cpr. These
currently include:

MOTE: While this library does now build on Microsoft Windows using MingW32 with
the pthread target, some features and functionality may still be limited or
disabled for that platform. As it depends on MingW32 unistd.h support it
probably cannot be built on windows with MSVC at all.

## cpr/bufio.h

Basic full duplex low level zero copy stream buffered I/O access to low
level file descriptors. This provides a low level buffered i/o concept similar
to bufio in golang. In addition, bufio handles tty descriptors by restoring
terminal settings at close, and performing shutdown for sockets.

## cpr/endian.h

Functions to store into and access memory pointer data by endian order.

## cpr/keyfile.h

Parses config files that may be broken into \[sections\] and have key=value key
pairs in each section.

## cpr/memio.h

Memory "I/O" patterned on bufio.  While bufio was meant to parse and support
TCP sockets and TTY sessions, memio offers a similar interface for low level
fixed blocks of memory. The idea is that one might use memio to parse UDP
packets in a similar way as bufio would offer for TCP streams, such as for
parsing SIP message packets.

## cpr/memory.h

This provides some safe memory functions to improve code quality, and a basic
implementation of shared ptr objects. With C11 atomics support the reference
counting is thread-safe and may be used to pass allocated heap objects between
threads.

## cpr/mempager.h

This provides a pager memory allocation system to create micro-heaps that can be
disposed efficiently all at once.

## cpr/multicast.h

Basic multicast socket operation. Currently it creates a socket that binds to and
operates under a specfied interface and includes helpers to join and drop multicast
groups.

## cpr/pipeline.h

A pipeline, much like a very simple go channel, to move object pointers between
a producer and consumer thread per C11 threading. If drop policy is used then
dropped packets in the pipeline are also free'd.

## cpr/service.h

Basic support for writing service daemons, including logging.

## cpr/socket.h

Basic support to sockets and some convenience functions for casting sockaddr and
sockaddr\_storage types. Some "missing" BSD functions, like disconnect, and
join and drop for multicast groups, also are provided.

## cpr/strchar.h

Enhanced string operations to cover those missing from the C standard library,
memory safe string operations.

## cpr/string.h

A pure simple pure C string type.

## cpr/system.h

Unix and posix standard system functions, platform specific functions and
headers. It also may offer some missing library functions, particularly for
mingw32 systems. Somewhat analogous to what the C++ filesystem header does.
This is the only header meant as an abstraction layer.

## src/sync.h

Cross-platform monotonic clocking and deadline timing.

## cpr/thread.h

Cross-platform threading support. For C11 systems which do not support C11
threading, it can map posix pthread support to C11 threads thru the header.
This is used for MingW32 and BSD systems where libc is not updated for C11. It
also includes support fir extra threading synchronization primitives such as
semaphores, rw conditional locking, and Golang style wait groups.
