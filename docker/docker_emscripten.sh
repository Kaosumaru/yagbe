#!/bin/sh
mkdir -p build
docker run --rm -v $(pwd):/src -t kaosumaru/emscripten:test ./docker/build.sh
