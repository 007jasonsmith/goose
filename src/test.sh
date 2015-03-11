set -e
set -x
clear
g++ \
    -I$GTEST_DIR/include \
    -I. \
    -std=c++11 \
    -pthread \
    -fno-stack-protector -Wall -Wextra \
    ./klib/base.cpp ./klib/new_printf_test.cpp ./klib/strings.cpp \
    ../libgtest.a \
    -o test
./test
