# Features

Libcpr consists of a series of individual and roughly stand-alone C header
files with inline static functions and simple C pre-procrssor macros that
typically are installed under and included from /usr/include/cpr. These
currently include:

## cpr/bufio.h

Basic low level stream and zero copy buffered I/O access to a low level file
descriptors. This provides a low level buffered i/o concept similar to bufio in
golang.

## cpr/clock.h

Hi resolution monotomic clocking functions and monotomic thread support. This
supports converting monotomic timepoints for use with poll, select, ppoll, and
pselect as well as with time\_t system time.

## cpr/endian.h

Functions to store into and access memory pointer data by endian order.

## cpr/keyfile.h

Parses config files that may be broken into \[sections\] and have key=value key
pairs in each section.

## cpr/memory.h

This provides some safe memory functions to improve code quality, and a basic
implimentation of shared ptr objects. With C11 atomics support the reference
counting is threadsafe and may be used to pass allocated heap objects between
threads.

## cpr/mempager.h

This provides a pager memory allocation system to create micro-heaps that can be
disposed efficiently all at once.

## cpr/openssl.h

Access to basic openssl crypto functions, especially common digests.

## cpr/socket.h

Basic support to sockets and some convience functions for casting sockaddr and
sockaddr\_storage types. Some "missing" BSD functions, like disconnect, and
join and drop for multicast groups, also are provided.

## cpr/strchar.h

Enhanced string operations to cover those missing from the C standard library,
memory safe string operations.

## cpr/string.h

A pure simple pure C string type.

## cpr/system.h

Unix and posix standard system functions, platform specific functions and
headers. Offers common mapping between posix error numbers and Windows error
codes. Somewhat analogous to what the C++ filesystem header does. This is the
only header meant as an abstraction layer.

## cpr/tty.h

Low level access to and control over posix tty serial devices.

## cpr/wolfssl.h

Some basic low level digest and support functions for using wolfssl.

