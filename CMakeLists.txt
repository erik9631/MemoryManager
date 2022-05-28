cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(RedBlackMemoryManager CXX)

add_subdirectory(libs/catch2)
include_directories(libs/catch2)
add_subdirectory(src)
target_link_libraries(RedBlackMemoryManager catch2Lib)