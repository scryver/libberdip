#!/bin/bash

set -e

curDir="$(pwd)"
codeDir="$curDir/src"
buildDir="$curDir/gebouw"

flags="-O0 -g -ggdb -Wall -Werror -pedantic -std=c++11"

exceptions="-Wno-unused-function -Wno-writable-strings -Wno-gnu-zero-variadic-macro-arguments -Wno-gnu-anonymous-struct -Wno-missing-braces -Wno-nested-anon-types"

echo "Building LIBBERDIP"

mkdir -p "$buildDir"

pushd "$buildDir" > /dev/null
    clang++ $flags $exceptions "$codeDir/fonts.cpp" -o font-convert
popd > /dev/null

