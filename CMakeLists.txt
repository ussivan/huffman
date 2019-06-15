cmake_minimum_required(VERSION 3.13)
project(huffman)

set(CMAKE_CXX_STANDARD 14)


add_library(huffman.lib STATIC huffman.cpp huffman.h)
add_executable(huffman huffman.cpp huffman.h console.cpp)
add_executable(test test.cpp)

target_link_libraries(huffman huffman.lib)
target_link_libraries(test huffman.lib)