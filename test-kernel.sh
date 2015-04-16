# Note: On a 64-bit system you need to install the following:
# apt-get install g++-multilib
clear

set -e
set -x

export GTEST_DIR=./third_party/googletest/

if [ -f ./bin/unit-test ]
  then
  rm ./bin/unit-test
fi

# Build GoogleTest if not found.
if [ ! -f ./bin/libgtest.a ]
  then
  echo "Building GoogleTest library."
  mkdir -p ./bin/
  g++ \
    -m32 \
    -isystem $GTEST_DIR/include \
    -I$GTEST_DIR \
    -pthread \
    -c $GTEST_DIR/src/gtest-all.cc

 ar -rv ./bin/libgtest.a gtest-all.o
fi

# TODO(chris): Move this to the Makefile.
echo "Building kernel unit tests."
g++ \
    -I$GTEST_DIR/include \
    -I. \
    -m32 \
    -std=c++11 \
    -pthread \
    -Wall -Wextra \
    ./klib/panic.cpp \
    ./kernel/boot.cpp \
    ./kernel/elf.cpp \
    ./kernel/memory.cpp \
    ./kernel/memory_test.cpp \
    ./kernel/tests_main.cpp \
    ./bin/libgtest.a \
    -o ./bin/unit-test

echo "Running."
./bin/unit-test
