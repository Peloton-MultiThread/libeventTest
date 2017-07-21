cmake_minimum_required(VERSION 3.5.1)
project(libeventClient)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(libeventClient ${SOURCE_FILES})