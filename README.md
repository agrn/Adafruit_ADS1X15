Adafruit_ADS1015
===

C library for TI's ADS1015: 12-bit Differential or Single-Ended ADC with PGA and Comparator.

This was originally a C++ library for Arduino, but I converted it to C and ported it to Linux.

This library only works on Linux. I don't have the time to port it to other platforms, but you can do a pull request.

How to build
---
There is a simple Makefile. No configure script, or autotools, or cmake, or any other tool, is required. To build the library and the examples, just clone the repo, cd to the folder, and run make.

There is other targets:
 * make all: build the library and the examples. This is the default.
 * make libs: build the library.
 * make examples: build the library and the examples, then delete the library.
 * make clean: clean the library, object files and examples.
 * make clean_after: build the library and the examples, then delete intermediate object files.
 * make clean_all_after: build the library and the examples, then delete the library and intermediate object files.
 * make mrproper: delete the build folder.

You can also use parameters:
 * CC=<compiler name>: change the compiler.
 * LD=<linker name>: change the linker.
 * CFLAGS=<compiler flags>: add flags for the compiler.
 * LDFLAGS=<linker flags>: add flags for the linker.
 * MODE=release: optimises and strips the binaries. Any warning encountered will result in an error.
 * BUILDDIR=<build directory>: change the output directory. Default to ./build/

By default, the compiler generates debugging symbols. Using MODE=release will disable them.

Dislaimer
---
This library is **not** endorsed by Adafruit. I needed a C library for the ADS1015, so I forked and ported their Arduino library to Linux.
