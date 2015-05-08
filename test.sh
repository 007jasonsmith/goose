# Note: On a 64-bit system you need to install the following:
# apt-get install g++-multilib
clear

set -e
set -x

export GTEST_DIR=./third_party/googletest/

# Clang conversion
# export CC=g++
export CC=clang++

# Build GoogleTest if not found.
# sudo apt-get install libc++6
echo "Building GoogleTest library."
mkdir -p ./bin/
$CC \
  -m32 \
  -pthread \
  -I $GTEST_DIR \
  -I $GTEST_DIR/include \
  -c $GTEST_DIR/src/gtest-all.cc

ar -rv ./bin/libgtest.a gtest-all.o

# TODO(chris): Move this to the Makefile.
echo "Building klib unit tests."
$CC \
    -I$GTEST_DIR/include \
    -I. \
    -std=c++11 \
    -pthread \
    -m32 \
    -fno-stack-protector -Wall -Wextra \
    ./klib/strings.cpp \
    ./klib/argaccumulator.cpp \
    ./klib/argaccumulator_test.cpp \
    ./klib/type_printer.cpp \
    ./klib/type_printer_test.cpp \
    ./klib/print_test.cpp \
    ./klib/debug.cpp \
    ./klib/debug_test.cpp \
    ./klib/tests_main.cpp \
    ./klib/print.cpp \
    ./bin/libgtest.a \
    -o ./bin/klib-tests

echo "Running."
./bin/klib-tests

echo "Building kernel unit tests."
$CC \
    -I$GTEST_DIR/include \
    -I. \
    -m32 \
    -std=c++11 \
    -pthread \
    -Wall -Wextra \
    ./klib/argaccumulator.cpp \
    ./klib/panic.cpp \
    ./klib/type_printer.cpp \
    ./klib/print.cpp \
    ./klib/strings.cpp \
    ./kernel/boot.cpp \
    ./kernel/elf.cpp \
    ./kernel/memory.cpp \
    ./kernel/memory_test.cpp \
    ./kernel/tests_main.cpp \
    ./bin/libgtest.a \
    -o ./bin/kernel-tests

echo "Running."
./bin/kernel-tests
