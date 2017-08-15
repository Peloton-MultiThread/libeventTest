cmake_minimum_required(VERSION 3.5.1)
project(manualActivateTest)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        threads.cpp
        main.cpp)

include_directories(include/)

link_directories(/usr/local/lib)

#find_library(RUNTIME_LIB NAMES libevent.a libevent_pthreads.a /usr/local/lib NO_DEFAULT_PATH)

#link_libraries(${RUNTIME_LIB})

add_executable(manualActivateTest ${SOURCE_FILES})

target_link_libraries(manualActivateTest pthread event_pthreads event)

