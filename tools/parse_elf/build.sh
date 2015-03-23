clear

rm *.o
rm parse_elf

set -e
set -x

g++ main.cpp -o parse_elf
