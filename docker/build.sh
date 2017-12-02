#!/bin/sh
mkdir -p build
cd build
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=make -G "Unix Makefiles"
emmake make
