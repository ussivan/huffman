cmake_minimum_required(VERSION 3.13)
project(huffman)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -ggdb")
endif ()


add_library(huffman.lib STATIC huffman.cpp huffman.h)
add_executable(huffman huffman.cpp huffman.h console.cpp)
add_executable(test test.cpp)

target_link_libraries(huffman huffman.lib)
target_link_libraries(test huffman.lib)