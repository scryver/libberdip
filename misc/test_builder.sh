#!/bin/bash

set -e

curDir="$(pwd)"
libDir="$curDir/src"
testDir="$curDir/test"
buildDir="$curDir/gebouw/test"

flags="-O0 -g -ggdb -Wall -Werror -pedantic"
cppFlags="$flags -std=c++11 -Wno-writable-strings"
cFlags="$flags -std=c99"

exceptions="-Wno-gnu-zero-variadic-macro-arguments -Wno-missing-braces -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-c99-extensions"
devExcept="-Wno-unused-function"

mkdir -p "$buildDir"

pushd "$buildDir" > /dev/null
    rm -f main_test.c main_test.cpp
    echo "#include \"$libDir/common.h\"" > main_test.c
    echo "#include \"$libDir/maps.h\"" >> main_test.c
    echo "#include \"$libDir/strings.h\"" >> main_test.c
    echo "#include \"$libDir/tests.h\"" >> main_test.c

    find "$testDir" -type f -name "*.c" -exec echo "#include \"{}\"" >> main_test.c \;

    echo "" >> main_test.c
    echo "int main(int argc, char **argv) {" >> main_test.c
    echo "    fprintf(stdout, \"Testing C-files of $testDir\\n\");" >> main_test.c

    find "$testDir" -type f -name "*.c" -exec sh -c "grep TEST_BEGIN {} | sed s\"/TEST_BEGIN(\(.*\))$/    testrun_\1();/g\" >> main_test.c" \;

    echo "    fprintf(stdout, \"\\n\");" >> main_test.c
    echo "}" >> main_test.c
    echo "" >> main_test.c


    echo "#include \"$libDir/platform.h\"" > main_test.cpp
    echo "#include \"$libDir/tests.h\"" >> main_test.cpp
    echo "#include \"$libDir/std_file.c\"" >> main_test.cpp

    find "$testDir" -type f -name "*.cpp" -exec echo "#include \"{}\"" >> main_test.cpp \;

    echo "" >> main_test.cpp
    echo "int main(int argc, char **argv) {" >> main_test.cpp
    echo "    fprintf(stdout, \"Testing C++-files of $testDir\\n\");" >> main_test.cpp

    find "$testDir" -type f -name "*.cpp" -exec sh -c "grep TEST_BEGIN {} | sed s\"/TEST_BEGIN(\(.*\))$/    testrun_\1();/g\" >> main_test.cpp" \;

    echo "    fprintf(stdout, \"\\n\");" >> main_test.cpp
    echo "}" >> main_test.cpp
    echo "" >> main_test.cpp

    clang $cFlags $exceptions $devExcept main_test.c -o c-test
    clang++ $cppFlags $exceptions $devExcept main_test.cpp -o cpp-test

    ./c-test
    ./cpp-test

popd > /dev/null

