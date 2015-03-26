rm unit-test
set -e
set -x
clear

export GTEST_DIR=../third_party/googletest/

g++ \
    -I$GTEST_DIR/include \
    -I. \
    -std=c++11 \
    -pthread \
    -fno-stack-protector -Wall -Wextra \
    ./klib/strings.cpp \
    ./klib/argaccumulator.cpp \
    ./klib/argaccumulator_test.cpp \
    ./klib/type_printer.cpp \
    ./klib/type_printer_test.cpp \
    ./klib/print_test.cpp \
    ./klib/tests_main.cpp \
    ../libgtest.a \
    -o unit-test

./unit-test
