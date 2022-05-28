cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(catch2Lib CXX)

add_library(catch2Lib INTERFACE)
target_include_directories(catch2Lib INTERFACE catch.hpp)