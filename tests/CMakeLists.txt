cmake_minimum_required(VERSION 3.22)

project(tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(u8_iterator u8_iterator.cpp ../source/utils.cpp)
target_include_directories(u8_iterator PRIVATE ../include)