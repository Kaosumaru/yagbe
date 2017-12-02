#!/bin/sh
docker run --rm -v $(pwd):/src -t kaosumaru/emscripten:test ./docker/build.sh
