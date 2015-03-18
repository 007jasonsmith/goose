rm unit-test
set -e
set -x
clear

g++ \
    -I$GTEST_DIR/include \
    -I. \
    -std=c++11 \
    -pthread \
    -fno-stack-protector -Wall -Wextra \
    ./klib/strings.cpp \
    ./klib/base.cpp \
    ./klib/argaccumulator.cpp \
    ./klib/argaccumulator_test.cpp \
    ./klib/typeprinter.cpp \
    ./klib/typeprinter_test.cpp \
    ./klib/new_printf_test.cpp \
    ./klib/tests_main.cpp \
    ../libgtest.a \
    -o unit-test

./unit-test
