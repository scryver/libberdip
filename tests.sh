#!/bin/bash

set -e

curDir="$(pwd)"
codeDir="$curDir/test"
buildDir="$curDir/gebouw"

flags="-O0 -g -ggdb -Wall -Werror -pedantic"
cppFlags="$flags -std=c++11 -Wno-writable-strings"
cFlags="$flags -std=c99"

exceptions="-Wno-gnu-zero-variadic-macro-arguments -Wno-missing-braces"
devExcept="-Wno-unused-function"

mkdir -p "$buildDir"

pushd "$buildDir" > /dev/null
    clang $cFlags $exceptions $devExcept "$codeDir/common_c.c" -o common-c
    clang++ $cppFlags $exceptions $devExcept "$codeDir/common_cpp.cpp" -o common-cpp

    ./common-c
    ./common-cpp
popd > /dev/null

echo "Done."
