#!/bin/sh
umask 0000
mkdir -p build
umask 0011s
cd build
emcmake cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=make -G "Unix Makefiles"
emmake make
