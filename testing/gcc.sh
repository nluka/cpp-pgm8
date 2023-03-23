g++ -g -Wall -Wextra -Wconversion -Wpedantic -Werror -std=c++20 *.cpp ../*.cpp -o pgm8.elf
mkdir -p files/with_comments
mkdir files/no_comments
valgrind ./pgm8.elf
