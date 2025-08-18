# About Libcpr

This is a package to collect and centralize testing for C17 headers and library
functions I had written and used to duplicate in other packages. These headers
are being gathered under a common package that can use libcpr as a core
dependency. Hence, one interpretation for for this package is "C Portable
Runtime".

One goal for CPR is to have minimal abstraction layers above C and the core
libc. There is a linkable library built for efficiency, but it has a very small
footprint. Static inlines are used to also limit what gets built or linked in
executables. The goal is to keep it small, simple, safe, and convient for
building system applications written in pure C.

Libcpr requires CMake and any modern C compiler to build it. Some C
applications may require the C11 threads library and so cannot be built on
Windows sytems. Otherise it should build and work with GCC (9 or later), with
Clang (14? or later), and perhaps even with MSVC. Besides GNU/Linux and BSD
systems, libcpr is portable to and can support the MingW platform target as
installed by Debian when selecting posix runtime, and this offers thread
support. The minimum requirement is a C17 compiler (or later).

## Dependencies

Libcpr applications may make use of C11 threading and atomics support.

## Distributions

Distributions of this package are provided as detached source tarballs made
from a tagged release from our internal source repository. These stand-alone
detached tarballs can be used to make packages for many GNU/Linux systems, and
for BSD ports. They may also be used to build and install the software directly
on a target platform.

## Licensing

Libcpr Copyright (C) 2025 David Sugar <tychosoft@gmail.comSug>,

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

## Participation

This project is offered as free (as in freedom) software for public use and has
a public home page at https://github.com/dyfet/libcpr which has an issue
tracker where people can submit public bug reports, and a wiki for hosting
project documentation. We are not maintaining a public git repo nor do we have
any production or development related resources hosted on external sites.
Patches may be submitted and attached to an issue in the issue tracker. Support
requests and other kinds of inquiries may also be sent privately thru email to
tychosoft@gmail.com. Other details about participation may be found in the
Contributing page.

## Testing

There are testing programs for each header. These run simple tests that will be
expanded to improve code coverage over time. The test programs are the only
built target making this library by itself, and the test programs in this
package work with the cmake ctest framework. They may also be used as simple
examples of how a given header works. There is also a **lint** target that can
be used to verify code changes.
