cmake_minimum_required(VERSION 3.5.1)
project(libeventTest)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp threads.cpp)

include_directories(include/)

link_directories(/usr/local/lib)

find_library(RUNTIME_LIB libevent.a /usr/local/lib NO_DEFAULT_PATH)

link_libraries(${RUNTIME_LIB})

add_executable(libeventTest ${SOURCE_FILES})

target_link_libraries(libeventTest pthread event_pthreads event)

